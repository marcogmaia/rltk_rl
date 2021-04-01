#include <memory>
#include "engine.hpp"
#include <fmt/format.h>
#include "core/factories.hpp"

#include "component/component.hpp"
#include "system/player_action.hpp"
#include "system/camera.hpp"
#include "utils/rng.hpp"

#include "spdlog/spdlog.h"


namespace radl::engine {

using namespace world;
using namespace rltk::colors;
// using namespace factory;
std::queue<sf::Event> event_queue;

entt::registry reg;
entt::entity player;
entt::entity map;

entt::observer observer{reg, entt::collector.group<position_t, player_t>()};

constexpr int width  = 96;
constexpr int height = 48;
static void rltk_init() {
    auto constexpr* font_file = "../assets";
    rltk::init(rltk::config_simple(font_file, width, height,
                                   "Maia Roguelike learning", "16x16", false));
}

void add_enemies() {
    constexpr uint32_t max_enemies_pex_room = 4;

    using rng::rng;

    auto w_map = reg.get<Map>(map);
    for(const auto& room : w_map.rooms) {
        auto num_enemies = rng.range(0, max_enemies_pex_room);
        for(int i = 0; i < num_enemies; ++i) {
            int rand_x          = rng.range(room.x1, room.x2 - 1);
            int rand_y          = rng.range(room.y1, room.y2 - 1);
            position_t rand_pos = {rand_x, rand_y};
            // TODO se pá posso fazer essa parada de ocupado com context
            // variables, mas não sei ainda como funcionam
            if(!is_occupied(reg, rand_pos)) {
                auto chance = rng.range(1, 3);
                if(chance == 1) {
                    factory::enemy_factory(w_map, rand_pos,
                                           vchar_t{'g', DARK_GREEN, BLACK});
                }
                else {
                    factory::enemy_factory(w_map, rand_pos,
                                           vchar_t{'O', GREEN, BLACK});
                }
            }
        }
    }
}

static void test_func(entt::registry& reg, entt::entity ent) {
    spdlog::warn("wtf ._ .");
}

void init() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    spdlog::info("Initializing engine.");
    rltk_init();
    reg.on_construct<game_status_t>().connect<&test_func>();
    reg.set<game_status_t>(game_status_t::STARTUP);

    reg.on_construct<player_t>().connect<&camera_update>();
    reg.on_update<player_t>().connect<&camera_update>();

    update();
}

void update() {
    static auto& gamestatus = reg.ctx<game_status_t>();
    switch(gamestatus) {
    case game_status_t::STARTUP: {
        auto map_obj = new_map(reg, rect_t{0, 0, width * 4, height * 4});
        auto player_start_pos = map_obj.rooms[0].center();
        map                   = reg.create();
        reg.emplace<world::Map>(map, std::move(map_obj));
        player = reg.create();
        factory::player_factory(player, player_start_pos,
                                vchar_t{'@', YELLOW, BLACK});
        add_enemies();
        gamestatus = game_status_t::IDLE;
    } break;
    case game_status_t::IDLE: {
        auto player_has_input = radl::process_input(reg, player);
        if(player_has_input) {
            gamestatus = game_status_t::NEW_TURN;
        }
    } break;
    case game_status_t::NEW_TURN: {
        // run everything else
        gamestatus = game_status_t::IDLE;
    } break;
    default:
        break;
    }
}

}  // namespace radl::engine
