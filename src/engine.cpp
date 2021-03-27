#include <engine.hpp>
#include <fmt/format.h>
#include "component/position.hpp"
#include "component/movable.hpp"
#include "component/vchar.hpp"
#include "component/renderable.hpp"
#include "component/player.hpp"

#include "player_action.hpp"

using namespace radl;


static void charset_init() {
    // auto constexpr* font_file = "../assets/tiles.png";
    // auto constexpr* font_file = "../assets/terminal12x12_gs_ro.png";
    auto constexpr* font_file = "../assets/terminal16x16_gs_ro.png";
    TCOD_console_set_custom_font(
        font_file, TCOD_FONT_LAYOUT_CP437 | TCOD_FONT_TYPE_GRAYSCALE, 16, 16);
    TCOD_console_map_ascii_code_to_font('.', 10, 15);
}

static void player_factory(entt::registry& reg, entt::entity& ent,
                           TCODConsole* console, const position_t& pos,
                           const radl::vchar_t& vchar) {
    reg.emplace<Player>(ent);
    reg.emplace<Movable>(ent);
    reg.emplace<position_t>(ent, pos);
    reg.emplace<Renderable>(ent, console, vchar);
}

static void map_factory(entt::registry& reg, entt::entity& ent,
                        const Renderable& rend, const Map& map) {
    reg.emplace<Renderable>(ent, rend);
    reg.emplace<Map>(ent, map);
}

static void render_map(entt::registry& reg, entt::entity& ent) {
    const auto& [rend, map] = reg.get<Renderable, Map>(ent);
    auto& console           = *rend.console;
    auto& dim               = map.dimensions;
    for(int x = 0; x < dim.w; ++x) {
        for(int y = 0; y < dim.h; ++y) {
            auto ch = map.tiles[x + y * dim.w].is_walkable ? '.' : '#';
            console.setChar(x, y, ch);
            console.setCharForeground(x, y, TCODColor::white);
        }
    }
}

void Engine::init() {
    constexpr int width  = 64;
    constexpr int height = 48;

    fmt::print("initializing engine.\n");
    charset_init();
    TCODConsole::initRoot(width, height, "Maia EnTT learning", false,
                          TCOD_renderer_t::TCOD_RENDERER_SDL2);
    TCODConsole::root->setDefaultBackground(TCODColor::darkestGrey);
    TCODSystem::setFps(60);

    Map tmap = Map(rect_t{0, 0, width, height});
    map      = reg.create();
    map_factory(
        reg, map,
        {TCODConsole::root, vchar_t{'x', TCODColor::white, TCODColor::white}},
        tmap);

    player = reg.create();
    player_factory(reg, player, TCODConsole::root,
                   position_t{width / 2, height / 2},
                   {'@', TCODColor::yellow, TCODColor::black});
}


void Engine::render() {
    TCODConsole::root->clear();
    // render map
    render_map(reg, map);
    // render enemies
    // render player
    const auto& [pos, rend] = reg.get<position_t, Renderable>(player);
    auto& console           = *rend.console;
    console.setCharForeground(pos.x, pos.y, rend.vchar.foreground);
    // console.setCharBackground(pos.x, pos.y, vch.background);
    console.setChar(pos.x, pos.y, rend.vchar.glyph);
}

void Engine::update() {
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &last_key, nullptr);
    // update player
    auto player_has_input = radl::process_input(reg, player, last_key);
    if(player_has_input) {
        // update world
    }
}
