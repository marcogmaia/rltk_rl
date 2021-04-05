#include <memory>
#include "engine.hpp"
#include "core/factories.hpp"

#include "component/component.hpp"
#include "system/player_action.hpp"
#include "system/camera.hpp"
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

void add_enemies() {
    constexpr uint32_t max_enemies_pex_room = 4;
    using rng::rng;

    const auto& w_map = reg.ctx<Map>();
    for(const auto& room : w_map.rooms) {
        auto num_enemies = rng.range(0, max_enemies_pex_room);
        for(int i = 0; i < num_enemies; ++i) {
            int rand_x          = rng.range(room.x1, room.x2 - 1);
            int rand_y          = rng.range(room.y1, room.y2 - 1);
            position_t rand_pos = {rand_x, rand_y};
            if(!is_occupied(reg, rand_pos)) {
                auto chance = rng.range(1, 3);
                if(chance == 1) {
                    factory::enemy_factory(
                        rand_pos, vchar_t{'g', DARK_GREEN, BLACK}, "Goblin");
                }
                else {
                    factory::enemy_factory(rand_pos, vchar_t{'O', GREEN, BLACK},
                                           "Orc");
                }
            }
        }
    }
}

void init() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    spdlog::info("Initializing engine.");
    rltk_init();
    reg.set<game_status_t>(game_status_t::STARTUP);

    update();
}

void update() {
    static auto& gamestatus = reg.ctx<game_status_t>();
    switch(gamestatus) {
    case game_status_t::STARTUP: {
        auto map_obj = new_map(reg, rect_t{0, 0, width * 4, height * 4});
        auto player_start_pos = map_obj.rooms[0].center();
        reg.set<Map>(map_obj);
        player = reg.create();
        factory::player_factory(player, player_start_pos,
                                vchar_t{'@', YELLOW, BLACK});
        add_enemies();
        query_entities_near_player();
        fov_update(reg, player);
        camera_update(reg, player);
        gamestatus = game_status_t::IDLE;
    } break;
    case game_status_t::IDLE: {
        while(!engine::event_queue.empty()) {
            auto player_has_input = radl::process_input(reg, player);
            if(player_has_input) {
                query_entities_near_player();
                fov_update(reg, player);
                fov_update_parallel(get_entities_near_player());
                ai_enemy(reg);
            }
        }
        // TODO move switch from process_input to here.
        gamestatus = game_status_t::NEW_TURN;
        // }
    } break;
    case game_status_t::NEW_TURN: {
        camera_update(reg, player);
        gamestatus = game_status_t::IDLE;
    } break;
    default:
        break;
    }
}

void terminate() {
    reg.clear();
}

}  // namespace radl::engine
