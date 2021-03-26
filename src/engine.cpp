#include <engine.hpp>
#include <fmt/format.h>
#include "position.hpp"
#include "movable.hpp"
#include "vchar.hpp"
#include "renderable.hpp"

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

void Engine::init() {
    fmt::print("initializing engine.\n");

    charset_init();

    TCODConsole::initRoot(64, 48, "Maia EnTT learning", false,
                          TCOD_renderer_t::TCOD_RENDERER_SDL2);
    TCODConsole::root->setDefaultBackground(TCODColor::darkestGrey);
    TCODSystem::setFps(60);


    player = reg.create();
    reg.emplace<position_t>(player, 0, 0);
    reg.emplace<Movable>(player);
    reg.emplace<Renderable>(player, TCODConsole::root);
    reg.emplace<vchar_t>(player, '@', TCODColor::yellow, TCODColor::black);
}


void Engine::render() {
    TCODConsole::root->clear();
    const auto& view = reg.view<position_t, vchar_t, Renderable>();
    const auto& [pos, vch, rend]
        = view.get<position_t, vchar_t, Renderable>(player);
    auto& console = *rend.console;
    console.setCharForeground(pos.x, pos.y, vch.foreground);
    // console.setCharBackground(pos.x, pos.y, vch.background);
    console.setChar(pos.x, pos.y, vch.glyph);
}

void Engine::update() {
    TCOD_key_t key;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr);
    // TODO arrumar esse nome, tá errado
    radl::process_input(reg, player, key);
}
