#include <engine.hpp>
#include <fmt/format.h>
#include "factories.hpp"

#include "component/component.hpp"

#include "system/player_action.hpp"
#include "system/camera.hpp"

#include "spdlog/spdlog.h"
namespace radl {
using namespace world;
using namespace rltk::colors;

entt::registry reg;
entt::entity player;
// world::Map map;
entt::entity map;

entt::observer observer{reg, entt::collector.group<position_t, player_t>()};


// static void map_factory(const renderable_t& rend, const world::Map& map_obj)
// {
//     reg.emplace<renderable_t>(map, rend);
//     reg.emplace<Map>(map, map_obj);
// }

// static void render_map(entt::registry& reg, world::Map& map) {
//     using namespace world;
//     // const auto& map = reg.get<Map>(ent_map);
//     // auto group_vis = map.reg.group<visible_t>();
//     // auto gp = map.reg.view<position_t>();
//     auto v = map.reg.view<position_t, tile_t>();
//     for(auto ent : v) {
//         auto [pos, ttype] = map.reg.get<position_t, tile_t>(ent);
//         auto [x, y]       = pos;
//         vchar_t vch       = {glyph::BLOCK1, WHITE, BLACK};
//         if(ttype.type == tile_type_t::wall) {
//             vch = {'#', BLACK, BLACK};
//         }
//         rltk::console->set_char(x, y, vch);
//     }
// }

constexpr int width  = 96;
constexpr int height = 48;
static void rltk_init() {
    auto constexpr* font_file = "../assets";
    rltk::init(rltk::config_simple(font_file, width, height,
                                   "Maia Roguelike learning", "16x16", false));
}

// void add_enemies() {
//     auto wmap = reg.get<world::Map>(map);
//     for(auto& room : wmap.rooms) {
//         auto enemy = reg.create();
//         reg.emplace<renderable_t>(enemy, vchar_t{'O', GREEN, BLACK});
//         reg.emplace<position_t>(enemy, room.center());
//         // reg.emplace<destructible_t>(enemy);
//     }
// }

void Engine::init() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    spdlog::info("Initializing engine.");
    rltk_init();

    auto map_obj          = new_map(rect_t{0, 0, width * 4, height * 4});
    auto player_start_pos = map_obj.rooms[0].center();

    map = reg.create();
    reg.emplace<world::Map>(map, std::move(map_obj));
    player = reg.create();
    player_factory(reg, player, player_start_pos, {'@', YELLOW, BLACK});
    // TODO enemies
    // add_enemies();
    update();
}

// static void render_player(entt::registry& reg, entt::entity& player) {
//     const auto& [pos, rend] = reg.get<position_t, renderable_t>(player);
//     auto& [x, y]            = pos;
//     using rltk::console;
//     console->set_char(x, y, rend.vchar);
// }

// void Engine::render() {
//     using rltk::console;
//     if(console->dirty) {
//         // console->clear();
//         // render entire screen
//         // render map
//         // render enemies
//         // TODO make enemies
//         // TODO render enemies
//         // render player
//         // render_player(reg, player);
//         // }
//     }
// }

void Engine::update() {
    // XXX resizeable console
    // update player
    auto player_has_input = radl::process_input(reg, player);
    if(player_has_input) {
        // TODO update world
    }
    // update_viewshed();
}

}  // namespace radl
