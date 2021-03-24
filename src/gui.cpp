#include <fmt/format.h>
#include "gui.hpp"
#include "engine.hpp"

Gui::Gui()
    : m_console(TCODConsole(engine::screen_width, PANEL_HEIGHT)) {
    // m_console
    //     = std::make_unique<TCODConsole>(engine::screen_width, PANEL_HEIGHT);
    // mconsol
}

Gui::~Gui() {
    // delete console;
}

Gui::Message::Message(const char* text, const TCODColor& color)
    : text(text)
    , color(color) {}

void Gui::render() {
    // Clear the GUI console
    m_console.setDefaultBackground(TCODColor::black);
    m_console.clear();
    // draw health bar
    const auto& player_status = engine::player->destructible;
    render_bar(position_t(1, 1), BAR_WIDTH, "HP", player_status->hp,
               player_status->max_hp, TCODColor::lightRed,
               TCODColor::darkerRed);
    TCODConsole::blit(&m_console, 0, 0, engine::screen_width, PANEL_HEIGHT,
                      TCODConsole::root, 0,
                      TCODConsole::root->getHeight() - PANEL_HEIGHT);
}

void Gui::render_bar(position_t pos, int width, const char* name, float value,
                     float max_value, const TCODColor& bar_color,
                     const TCODColor& back_color) {
    // fill the background
    m_console.setDefaultBackground(back_color);
    m_console.rect(pos.x, pos.y, width, 1, false,
                   TCOD_bkgnd_flag_t::TCOD_BKGND_SET);

    auto bar_width = static_cast<int>(value / max_value * width);
    if(bar_width > 0) {
        // draw the bar
        m_console.setDefaultBackground(bar_color);
        m_console.rect(pos.x, pos.y, bar_width, 1, false,
                       TCOD_bkgnd_flag_t::TCOD_BKGND_SET);
    }
    // print text on top of the bar
    m_console.setDefaultForeground(TCODColor::white);
    m_console.printEx(
        pos.x + width / 2, pos.y, TCOD_bkgnd_flag_t::TCOD_BKGND_NONE,
        TCOD_alignment_t::TCOD_CENTER, "%s : %g/%g", name, value, max_value);
}