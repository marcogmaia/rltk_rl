#include <engine.hpp>
#include <fmt/format.h>
#include "component/position.hpp"
#include "component/movable.hpp"
#include "component/vchar.hpp"
#include "component/renderable.hpp"
#include "component/player.hpp"

#include "player_action.hpp"

#include "spdlog/spdlog.h"
namespace radl {

entt::registry reg;
entt::entity player;
entt::entity map;

static void charset_init() {
    // auto constexpr* font_file = "../assets/tiles.png";
    // auto constexpr* font_file = "../assets/terminal12x12_gs_ro.png";
    auto constexpr* font_file = "../assets/terminal16x16_gs_ro.png";
    TCOD_console_set_custom_font(
        font_file, TCOD_FONT_LAYOUT_CP437 | TCOD_FONT_TYPE_GRAYSCALE, 16, 16);
    TCOD_console_map_ascii_code_to_font('.', 10, 15);
}

static void player_factory(TCODConsole* console, const position_t& pos,
                           const radl::vchar_t& vchar) {
    reg.emplace<Player>(player);
    reg.emplace<Movable>(player);
    reg.emplace<position_t>(player, pos);
    reg.emplace<Renderable>(player, console, vchar);
}

static void map_factory(const Renderable& rend, const Map& map_obj) {
    reg.emplace<Renderable>(map, rend);
    reg.emplace<Map>(map, map_obj);
}

static void render_map(entt::registry& reg, entt::entity& ent) {
    const auto& [rend, map] = reg.get<Renderable, Map>(ent);
    auto& console           = *rend.console;
    auto& rect              = map.rect;
    for(int x = rect.x1; x < rect.x2; ++x) {
        for(int y = rect.y1; y < rect.y2; ++y) {
            auto& is_walkable = map.at(x, y).is_walkable;
            auto ch           = is_walkable ? '.' : '#';
            auto fg = is_walkable ? TCODColor::grey : TCODColor::darkestGrey;
            auto bg = is_walkable ? TCODColor::grey : TCODColor::black;
            console.setChar(x, y, ch);
            console.setCharBackground(x, y, bg);
            console.setCharForeground(x, y, fg);
        }
    }
}

void Engine::init() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    constexpr int width  = 64;
    constexpr int height = 48;

    fmt::print("initializing engine.\n");
    charset_init();
    TCODConsole::initRoot(width, height, "Maia EnTT learning", false,
                          TCOD_renderer_t::TCOD_RENDERER_SDL2);
    TCODConsole::root->setDefaultBackground(TCODColor::darkestGrey);
    TCODSystem::setFps(90);

    position_t player_start_pos = position_t{width / 2, height / 2};

    Map tmap = new_map(rect_t{0, 0, width, height}, player_start_pos);
    map      = reg.create();
    map_factory(
        {TCODConsole::root, vchar_t{'x', TCODColor::white, TCODColor::white}},
        tmap);

    player = reg.create();
    player_factory(TCODConsole::root, player_start_pos,
                   {'@', TCODColor::yellow, TCODColor::black});
}

static void render_player(entt::registry& reg, entt::entity& player) {
    const auto& [pos, rend] = reg.get<position_t, Renderable>(player);
    auto& console           = *rend.console;
    console.setCharForeground(pos.x, pos.y, rend.vchar.foreground);
    console.setChar(pos.x, pos.y, rend.vchar.glyph);
}

void Engine::render() {
    TCODConsole::root->clear();
    // render map
    render_map(reg, map);
    // render enemies
    // TODO render enemies
    // render player
    render_player(reg, player);
}

void Engine::update() {
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &last_key, nullptr);
    // update player
    auto player_has_input = radl::process_input(reg, player, last_key);
    if(player_has_input) {
        // update world
    }
}

}  // namespace radl