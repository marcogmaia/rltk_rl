#include <engine.hpp>
#include <fmt/format.h>
#include "component/position.hpp"
#include "component/movable.hpp"
#include "component/vchar.hpp"
#include "component/renderable.hpp"
#include "component/player.hpp"
#include "component/viewshed.hpp"

#include "system/player_action.hpp"

#include "spdlog/spdlog.h"
namespace radl {
using namespace world;
using namespace rltk::colors;

entt::registry reg;
entt::entity player;
entt::entity map;

entt::observer observer;  //{reg, entt::collector.groupe<position_t, Player>()};


// void update_viewshed() {
//     observer.each([](auto ent) {
//         auto& vshed  = reg.get<viewshed_t>(ent);
//         auto& [x, y] = reg.get<position_t>(ent);
//         spdlog::debug("range view: {}", vshed.range);
//         spdlog::debug("pos: ({}, {})", x, y);
//     });
// }

void update_viewshed(entt::registry& r, entt::entity e) {
    auto& [x, y] = r.get<position_t>(e);
    spdlog::debug("position ({}, {})", x, y);
}

static void player_factory(const position_t& pos, const radl::vchar_t& vch) {
    reg.on_construct<position_t>().connect<&update_viewshed>();
    reg.on_update<position_t>().connect<&update_viewshed>();
    reg.emplace<Player>(player);
    reg.emplace<viewshed_t>(player);
    reg.emplace<Movable>(player);
    reg.emplace<position_t>(player, pos);
    reg.emplace<Renderable>(player, Renderable{vch});
    // TODO observer ?
}

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

    // XXX I can remove this variable, and refactor the calls
    position_t player_start_pos = position_t{width / 2, height / 2};

    Map tmap = world::new_map(rect_t{0, 0, width, height}, player_start_pos);
    player_start_pos = tmap.rooms[0].center();
    map              = reg.create();
    map_factory(Renderable{{'x', DARKEST_GREY, WHITE}}, tmap);

    player = reg.create();
    player_factory(player_start_pos, {'@', YELLOW, BLACK});
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
        console->clear();
        // render entire screen
        // render map
        render_map(reg, map);
        // render_map(reg, map);
        // render enemies
        // TODO make enemies
        // TODO render enemies
        // render player
        render_player(reg, player);
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
