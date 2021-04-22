#include <memory>

#include "spdlog/spdlog.h"


#include "core/game_state.hpp"
#include "core/engine.hpp"
#include "core/gui/gui.hpp"
#include "core/spawner.hpp"
#include "system/factories.hpp"

#include "system/camera.hpp"
#include "system/player_action.hpp"
#include "system/system.hpp"

#include "utils/rng.hpp"


#include "gsl/gsl_util"

namespace radl::engine {

using namespace rltk::colors;
std::deque<sf::Event> event_queue;

rltk::virtual_terminal* console;

entt::dispatcher event_dispatcher{};
entt::sigh<sf::Event()> ev_signal{};
entt::sink ev_sink{ev_signal};

constexpr int width  = 1024 / 16;
constexpr int height = 768 / 16;

namespace {

using rltk::gui;
using rltk::term;

}  // namespace

static void rltk_init() {
    constexpr auto font_file = "../assets";
    rltk::init(
        rltk::config_advanced(font_file, width * 16, height * 16, "MaiaRL"));
}


namespace {}  // namespace

void init() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    spdlog::info("Initializing engine.");
    event_dispatcher.sink<double>().connect<&update>();

    
    reg.set<game_state_t>(game_state_t::PRE_RUN);
    rltk_init();
    gui::init();
}


void pre_run() {
    // initialize everything
    // create the map
    system::init_systems();
    reg.set<Map>();
    auto& map = get_map();
    map.init(rect_t{0, 0, width, height});
    create_random_rooms(map);
    make_corridors_between_rooms(map);
    auto player_start_pos = map.rooms[0].center();
    player                = reg.create();
    factory::player_factory(player, player_start_pos,
                            vchar_t{'@', YELLOW, BLACK});
    add_enemies();

    game_state_init();

    spdlog::info("entities created: {}", reg.alive());
    system::systems_run();
}

void restart_game_state() {
    reg.unset<Map>();
    reg.unset<game_log_t>();
    reg.clear();
}

void game_state_update([[maybe_unused]] double elapsed_time) {
    auto& game_state = reg.ctx<game_state_t>();

    switch(game_state) {
    case game_state_t::PRE_RUN: {
        pre_run();
        game_state = game_state_t::AWAITING_INPUT;
    } break;

    case game_state_t::AWAITING_INPUT: {
        game_state = system::player_input();
    } break;

    case game_state_t::SHOW_INVENTORY: {
        using gui::item_menu_result_t;
        auto [menu_res, ent] = gui::render_inventory_use();
        auto& inv_ui         = *term(gui::UI_INVENTORY_POPUP);

        switch(menu_res) {
        case item_menu_result_t::CANCEL: {
            inv_ui.clear();
            game_state = game_state_t::AWAITING_INPUT;
        } break;
        case item_menu_result_t::NO_RESPONSE: break;
        case item_menu_result_t::SELECTED: {
            inv_ui.clear();
            reg.emplace<wants_to_use_t>(player, ent);
            game_state = game_state_t::PLAYER_TURN;
        } break;
        }
    } break;

    case game_state_t::SHOW_INVENTORY_DROP: {
        using gui::item_menu_result_t;
        auto [menu_res, ent] = gui::render_inventory_drop();
        auto& inv_ui         = *term(gui::UI_INVENTORY_POPUP);

        switch(menu_res) {
        case item_menu_result_t::CANCEL: {
            inv_ui.clear();
            game_state = game_state_t::AWAITING_INPUT;
        } break;
        case item_menu_result_t::NO_RESPONSE: break;
        case item_menu_result_t::SELECTED: {
            inv_ui.clear();
            reg.emplace<wants_to_drop_t>(player, ent);
            game_state = game_state_t::PLAYER_TURN;
        } break;
        }
    } break;

    case game_state_t::PLAYER_TURN: {
        system::systems_run();
        game_state = game_state_t::ENEMY_TURN;
    } break;

    case game_state_t::ENEMY_TURN: {
        // ## if player is dead then restart game
        system::systems_run();

        if(reg.all_of<dead_t>(player)) {
            game_state = game_state_t::DEFEAT;
        } else {
            game_state = game_state_t::AWAITING_INPUT;
        }
    } break;

    case game_state_t::DEFEAT: {
        restart_game_state();
        game_state = game_state_t::PRE_RUN;
    } break;

    default: break;
    }
}

void phase_mouse_cursor(double elapsed_time) {
    static double alpha_cursor = 0.0;
    static double time_frame   = 0.0;
    static bool increasing     = true;
    time_frame += elapsed_time * 0.42;

    if(time_frame >= 1.0) {
        auto inc = static_cast<int>(time_frame);
        time_frame -= inc;
        if(increasing) {
            alpha_cursor += inc;
        } else {
            alpha_cursor -= inc;
        }
        if(alpha_cursor >= 255) {
            increasing   = false;
            alpha_cursor = 255;
        }
        if(alpha_cursor <= 0) {
            increasing   = true;
            alpha_cursor = 0;
        }
    }
    term(gui::UI_MOUSE)->set_alpha(static_cast<int>(alpha_cursor));
}

/**
 * @brief update the game state and run the systems
 *
 * @param elapsed_time
 */
void update(double elapsed_time) {
    engine::event_dispatcher.update();
    game_state_update(elapsed_time);
    phase_mouse_cursor(elapsed_time);
}


void terminate() {
    restart_game_state();
    gui::terminate();
}


}  // namespace radl::engine

namespace radl::state {

bool window_focused = true;
std::array<bool, 7> mouse_button_pressed;
int mouse_x = 0;
int mouse_y = 0;


void set_mouse_button_state(const int button, const bool state) {
    gsl::at(mouse_button_pressed, button) = state;
}

bool get_mouse_button_state(const int button) {
    return gsl::at(mouse_button_pressed, button);
}

double scale_factor = 1.0;
void set_mouse_position(const int x, const int y) {
    mouse_x = static_cast<int>(x / scale_factor);
    mouse_y = static_cast<int>(y / scale_factor);
}

position_t get_mouse_position() {
    return {mouse_x, mouse_y};
}

position_t get_mouse_position_coord() {
    auto [mx, my] = state::get_mouse_position();
    auto [fw, fh] = term(gui::UI_MOUSE)->get_font_size();
    mx /= fw;
    my /= fh;
    return {mx, my};
}

void reset_mouse_state() {
    std::fill(mouse_button_pressed.begin(), mouse_button_pressed.end(), false);
    mouse_x = 0;
    mouse_y = 0;
}

}  // namespace radl::state
