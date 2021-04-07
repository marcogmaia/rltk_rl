#include <memory>
#include "engine.hpp"
#include "core/factories.hpp"

#include "component/component.hpp"
#include "system/player_action.hpp"
#include "system/camera.hpp"
#include "system/system.hpp"
#include "utils/rng.hpp"

#include "spdlog/spdlog.h"

namespace radl::engine {

using namespace world;
using namespace rltk::colors;
std::queue<sf::Event> event_queue;

entt::registry reg;
entt::entity player;

rltk::virtual_terminal* console;
// sf::RenderWindow* main_window;

constexpr int width  = 1024 / 16;
constexpr int height = 768 / 16;

constexpr int PLAYER_SCREEN = 0;
constexpr int TEST_BOX      = 1;


namespace {

using rltk::gui;
using rltk::term;

void resize_main(rltk::layer_t* l, int w, int h) {
    // Simply set the width to the whole window width
    l->w = w;
    l->h = h;
}

void resize_box(rltk::layer_t* l, int w, int h) {
    // Simply set the width to the whole window width
    auto [fw, fh] = term(TEST_BOX)->get_font_size();

    l->w = 15 * fw;
    l->h = h - 2 * fh;
}

}  // namespace

static void rltk_init() {
    constexpr auto font_file = "../assets";
    rltk::init(rltk::config_advanced(font_file, width * 16, height * 16,
                                     "Maia Roguelike learning"));
    rect_t srect = {
        1 * 12,
        1 * 12,
        12 + 144,
        768,
    };
    rect_t mrect = {
        srect.x1,
        srect.y1,
        1024,
        768,
    };
    // XXX fix this to sync with one variable
    gui->add_layer(PLAYER_SCREEN, mrect.x1, mrect.y1, mrect.x2, mrect.y2,
                   "16x16", resize_main, true);
    gui->add_layer(TEST_BOX, srect.x1, srect.y1, srect.x2, srect.y2, "12x12",
                   resize_box, true);
    console = term(PLAYER_SCREEN);
}

void print_hp_bar(int x, int y, int hp_index, vchar_t fg_vch, vchar_t bg_vch) {
    [[maybe_unused]] auto [fw, fh] = term(TEST_BOX)->get_font_size();
    for(int i = 0; i < 10; ++i) {
        term(TEST_BOX)->set_char(x + 3 + i, y, bg_vch);
    }
    for(int i = 0; i < hp_index; ++i) {
        term(TEST_BOX)->set_char(1 + 3 + i, 3, fg_vch);
    }
}

void render_gui() {
    [[maybe_unused]] auto [fw, fh] = term(TEST_BOX)->get_font_size();
    term(TEST_BOX)->clear();
    term(TEST_BOX)->box(GREY, BLACK, true);
    const auto& pstats = reg.get<combat_stats_t>(player);
    term(TEST_BOX)->print(2, 2, fmt::format("{}/{}", pstats.hp, pstats.max_hp));
    term(TEST_BOX)->print(1, 3, "hp:");

    auto hp_index = static_cast<int>(std::ceil(pstats.hp / 10.0));
    print_hp_bar(1, 3, hp_index,
                 vchar_t{
                     glyph::BLOCK2,
                     RED,
                     BLACK,
                 },
                 vchar_t{
                     glyph::BLOCK2,
                     DARKEST_RED,
                     BLACK,
                 });


    term(TEST_BOX)->print(3, 0, "stats");
}

namespace {

void add_enemy(const position_t& pos) {
    if(!is_occupied(pos)) {
        auto chance = rng::rng.range(1, 3);
        if(chance == 1) {
            factory::enemy_factory(pos, vchar_t{'g', DARK_GREEN, BLACK},
                                   "Goblin");
        }
        else {
            factory::enemy_factory(pos, vchar_t{'O', GREEN, BLACK}, "Orc");
        }
    }
}

void add_enemies() {
    constexpr uint32_t max_enemies_pex_room = 4 * 10;
    using rng::rng;

    const auto& w_map = engine::get_map();
    for(const auto& room : w_map.rooms) {
        auto num_enemies = rng.range(0, max_enemies_pex_room);
        for(int i = 0; i < num_enemies; ++i) {
            int rand_x          = rng.range(room.x1, room.x2 - 1);
            int rand_y          = rng.range(room.y1, room.y2 - 1);
            position_t rand_pos = {rand_x, rand_y};
            if(!is_occupied(rand_pos)) {
                add_enemy(rand_pos);
            }
        }
    }
}
}  // namespace

void init() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    spdlog::info("Initializing engine.");
    rltk_init();
    reg.set<game_state_t>(game_state_t::STARTUP);
}

/**
 * @brief Get the map object
 *
 * @warning call this function only after setting the map on the register
 *
 * @return the map from the register context
 */
world::Map& get_map() {
    return reg.ctx<world::Map>();
}

namespace {


[[maybe_unused]] void enemy_spawner() {
    auto& map        = engine::get_map();
    int random_index = rng::rng.range(0, map.rooms.size() - 1);
    auto random_pos  = map.rooms.at(random_index).random_pos();
    add_enemy(random_pos);
}


}  // namespace


void update() {
    auto& gamestatus = reg.ctx<game_state_t>();
    switch(gamestatus) {
    case game_state_t::STARTUP: {
        reg.set<Map>();
        auto& map = get_map();
        map.init(rect_t{0, 0, width * 4, height * 4});
        create_random_rooms(map);
        make_corridors_between_rooms(map);

        auto player_start_pos = map.rooms[0].center();
        player                = reg.create();
        factory::player_factory(player, player_start_pos,
                                vchar_t{'@', YELLOW, BLACK});
        add_enemies();
        query_entities_near_player();
        fov_update();

        camera_update(reg, player);
        render_gui();

        spdlog::info("entities created: {}", reg.alive());
        gamestatus = game_state_t::PLAYER_TURN;
    } break;

    case game_state_t::PLAYER_TURN: {
        auto valid_input = radl::process_input(player);
        // perform action
        if(valid_input) {
            system::systems_player();
            gamestatus = game_state_t::ENEMY_TURN;
        }
    } break;
    case game_state_t::ENEMY_TURN: {
        // ## if player is dead then restart game
        query_entities_near_player();
        system::systems_run();

        camera_update(reg, player);

        // ## update gui
        render_gui();


        if(reg.all_of<dead_t>(player)) {
            gamestatus = game_state_t::DEFEAT;
        }
        else {
            gamestatus = game_state_t::PLAYER_TURN;
        }
    } break;
    case game_state_t::DEFEAT: {
        terminate();
        gamestatus = game_state_t::STARTUP;
    } break;
    default:
        break;
    }
}

void terminate() {
    reg.unset<world::Map>();
    reg.clear();
}

}  // namespace radl::engine
