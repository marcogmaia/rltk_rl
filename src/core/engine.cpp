#include <memory>

#include "spdlog/spdlog.h"

#include "component/component.hpp"

#include "core/engine.hpp"
#include "core/gui.hpp"
#include "core/spawner.hpp"
#include "core/factories.hpp"

#include "system/camera.hpp"
#include "system/player_action.hpp"
#include "system/system.hpp"

#include "utils/rng.hpp"


namespace radl::engine {

using namespace rltk::colors;
std::deque<sf::Event> event_queue;

entt::registry reg;
entt::entity player;

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
    rltk::init(rltk::config_advanced(font_file, width * 16, height * 16,
                                     "MaiaRL"));

    // XXX fix this to sync with one variable
}


namespace {

}  // namespace

void init() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    event_dispatcher.sink<sf::Event>().connect<&system::player_system>();
#endif
    spdlog::info("Initializing engine.");
    rltk_init();
    gui::init();
    reg.set<game_state_t>(game_state_t::PRE_RUN);
    // event_sink.connect<&process_input>();
}

/**
 * @brief Get the map from the register context
 *
 * @warning call this function only after setting the map on the register
 *
 * @return the map from the register context
 */
Map& get_map() {
    return reg.ctx<Map>();
}

component::game_log_t& get_game_log() {
    return reg.ctx<component::game_log_t>();
}

void game_state_system([[maybe_unused]] double elapsed_time) {
    auto& game_state = reg.ctx<game_state_t>();


    switch(game_state) {
    case game_state_t::PRE_RUN: {
        // initialize everything
        reg.set<Map>();
        reg.set<component::game_log_t>();
        auto& map = get_map();
        map.init(rect_t{0, 0, width * 4, height * 4});
        create_random_rooms(map);
        make_corridors_between_rooms(map);
        auto player_start_pos = map.rooms[0].center();
        player                = reg.create();
        factory::player_factory(player, player_start_pos,
                                vchar_t{'@', YELLOW, BLACK});

        add_enemies();
        spdlog::info("entities created: {}", reg.alive());
        system::systems_run();
        game_state = game_state_t::AWAITING_INPUT;
    } break;

    case game_state_t::AWAITING_INPUT: {
        render();
        game_state = player_input();
    } break;

    case game_state_t::SHOW_INVENTORY: {
        game_state = game_state_t::AWAITING_INPUT;
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
        terminate();
        game_state = game_state_t::PRE_RUN;
    } break;

    default: break;
    }
}

void update(double elapsed_time) {
    engine::event_dispatcher.update();
    game_state_system(elapsed_time);
}

void render() {
    if(reg.valid(player)) {
        camera_update(player);
        gui::render_gui();
    }
}

void terminate() {
    reg.unset<Map>();
    reg.unset<component::game_log_t>();
    reg.clear();
}

position_t get_position_from_entity(entity ent) {
    auto pos = reg.get<position_t>(ent);
    return pos;
}

}  // namespace radl::engine

namespace radl::state {

bool window_focused = true;
std::array<bool, 7> mouse_button_pressed;
int mouse_x = 0;
int mouse_y = 0;


void set_mouse_button_state(const int button, const bool state) {
    mouse_button_pressed[button] = state;
}

bool get_mouse_button_state(const int button) {
    return mouse_button_pressed[button];
}

double scale_factor = 1.0;
void set_mouse_position(const int x, const int y) {
    mouse_x = static_cast<int>(x / scale_factor);
    mouse_y = static_cast<int>(y / scale_factor);
}

position_t get_mouse_position() {
    return {mouse_x, mouse_y};
}

void reset_mouse_state() {
    std::fill(mouse_button_pressed.begin(), mouse_button_pressed.end(), false);
    mouse_x = 0;
    mouse_y = 0;
}

}  // namespace radl::state
