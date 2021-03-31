#include <memory>
#include "engine.hpp"
#include <fmt/format.h>
#include "factories.hpp"

#include "component/component.hpp"
#include "system/player_action.hpp"
#include "system/camera.hpp"
#include "utils/rng.hpp"

#include "spdlog/spdlog.h"


namespace radl {

using namespace world;
using namespace rltk::colors;

entt::registry reg;
entt::entity player;
// world::Map map;
entt::entity map;

entt::observer observer{reg, entt::collector.group<position_t, player_t>()};

std::unique_ptr<rltk::virtual_terminal> view_console;

constexpr int width  = 96;
constexpr int height = 48;
static void rltk_init() {
    auto constexpr* font_file = "../assets";
    rltk::init(rltk::config_simple(font_file, width, height,
                                   "Maia Roguelike learning", "16x16", false));
    //    rltk::console->set_alpha(50);
    // view_console = std::make_unique<rltk::virtual_terminal>(
    //     rltk::config.root_font, 0, 0);


    // sf::Vector2u size_pixels = main_window->getSize();
    // view
    // view_console->resize_pixels(size_pixels.x, size_pixels.y);
}

void add_enemies() {
    constexpr uint32_t max_enemies_pex_room = 4;

    auto& rng = rng::rng;

    auto w_map = reg.get<Map>(map);
    for(const auto& room : w_map.rooms) {
        auto num_enemies = rng.range(0, max_enemies_pex_room);
        for(int i = 0; i < num_enemies; ++i) {
        }
        auto enemy = reg.create();
        reg.emplace<renderable_t>(enemy, vchar_t{'O', GREEN, BLACK});
        auto pos = room.center();
        reg.emplace<position_t>(enemy, pos);
        reg.emplace<blocks_t>(enemy);
        reg.emplace<destructible_t>(enemy);
        // add to tile
        auto& tile_ent = w_map.at(pos);
        auto& tile     = reg.get<tile_t>(tile_ent);
        tile.entities.push_back(enemy);
    }
}

void Engine::init() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    spdlog::info("Initializing engine.");
    rltk_init();

    auto map_obj          = new_map(reg, rect_t{0, 0, width * 4, height * 4});
    auto player_start_pos = map_obj.rooms[0].center();

    map = reg.create();
    reg.emplace<world::Map>(map, std::move(map_obj));
    player = reg.create();
    reg.on_construct<player_t>().connect<&camera_update>();
    reg.on_update<player_t>().connect<&camera_update>();
    player_factory(reg, player, player_start_pos, {'@', YELLOW, BLACK});
    add_enemies();
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
//         // render player
//         // render_player(reg, player);
//         // }
//     }
// }

void Engine::update() {
    // static bool first_run = true;
    // if(first_run) {
    //     camera_update(reg, player);
    //     first_run = false;
    // }
    // XXX trigger redraw when sfml resize
    // update player
    auto player_has_input = radl::process_input(reg, player);
    if(player_has_input) {
        // TODO update world
        // camera_update(reg, player);
    }
    // update_viewshed();
}

}  // namespace radl
