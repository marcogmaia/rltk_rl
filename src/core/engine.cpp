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

constexpr int width  = 96;
constexpr int height = 48;
static void rltk_init() {
    // using namespace entt::literals;
    constexpr auto font_file = "../assets";
    rltk::init(rltk::config_simple(font_file, width, height,
                                   "Maia Roguelike learning", "16x16", false));
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


void enemy_spawner() {
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

        spdlog::info("entities created: {}", reg.alive());
        gamestatus = game_state_t::IDLE;
    } break;

    case game_state_t::IDLE: {
        auto valid_input = radl::process_input(player);
        // perform action
        if(valid_input) {
            system::systems_player();
            gamestatus = game_state_t::NEW_TURN;
        }
    } break;
    case game_state_t::NEW_TURN: {
        // ## if player is dead then restart game
        query_entities_near_player();
        system::systems_run();
        camera_update(reg, player);

        // ## update gui

        if(reg.all_of<dead_t>(player)) {
            gamestatus = game_state_t::DEFEAT;
        }
        else {
            gamestatus = game_state_t::IDLE;
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
