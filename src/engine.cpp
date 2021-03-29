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
entt::entity map;

entt::observer observer{reg, entt::collector.update<position_t>()};


// void update_viewshed() {
//     observer.each([](auto ent) {
//         auto& vshed  = reg.get<viewshed_t>(ent);
//         auto& [x, y] = reg.get<position_t>(ent);
//         spdlog::debug("range view: {}", vshed.range);
//         spdlog::debug("pos: ({}, {})", x, y);
//     });
// }


static void map_factory(const Renderable& rend, const world::Map& map_obj) {
    reg.emplace<Renderable>(map, rend);
    reg.emplace<Map>(map, map_obj);
}

static void render_map(entt::registry& reg, entt::entity& ent) {
    const auto& [rend, map] = reg.get<Renderable, Map>(ent);
    auto& rect              = map.rect;
    for(int x = rect.x1; x < rect.x2; ++x) {
        for(int y = rect.y1; y < rect.y2; ++y) {
            auto& is_walkable = map.at(x, y).is_walkable;
            auto fg           = is_walkable ? GREY : DARK_GREY;
            auto bg           = is_walkable ? GREY : BLACK;
            auto ch = is_walkable ? vchar_t{'.', fg, bg} : vchar_t{'#', fg, bg};
            rltk::console->set_char(x, y, ch);
        }
    }
}

constexpr int width  = 96;
constexpr int height = 48;
static void rltk_init() {
    auto constexpr* font_file = "../assets";
    rltk::init(rltk::config_simple(font_file, width, height,
                                   "Maia Roguelike learning", "16x16", false));
}

void Engine::init() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    fmt::print("initializing engine.\n");
    rltk_init();
    Map tmap = world::new_map(rect_t{0, 0, width * 4, height * 4});
    auto player_start_pos = tmap.rooms[0].center();
    map                   = reg.create();
    map_factory(Renderable{{'x', DARKEST_GREY, WHITE}}, tmap);
    player = reg.create();
    player_factory(reg, player, player_start_pos, {'@', YELLOW, BLACK});
    update();
}

static void render_player(entt::registry& reg, entt::entity& player) {
    const auto& [pos, rend] = reg.get<position_t, Renderable>(player);
    auto& [x, y]            = pos;
    using rltk::console;
    console->set_char(x, y, rend.vchar);
}

void Engine::render() {
    using rltk::console;
    if(console->dirty) {
        // console->clear();
        // render entire screen
        // render map
        // render_map(reg, map);
        // render_map(reg, map);
        // render enemies
        // TODO make enemies
        // TODO render enemies
        // render player
        // render_player(reg, player);
    }
}

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
