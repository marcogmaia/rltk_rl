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
    
Gui::Message::Message(const std::string& text, const TCODColor& color)
    : text(text)
    , color(color) {}

Gui::Message::Message(const std::string&& text, const TCODColor& color)
    : text(text)
    , color(color) {}

void Gui::render_messages() {
    int y           = 1;
    float colorCoef = 0.4f;
    for(auto& message : m_log) {
        m_console.setDefaultForeground(message.color * colorCoef);
        m_console.print(MSG_X, y, message.text);
        y++;
        if(colorCoef < 1.0f) {
            colorCoef += 0.3f;
        }
    }
}

void Gui::render() {
    // Clear the GUI console
    m_console.setDefaultBackground(TCODColor::black);
    m_console.clear();
    // draw health bar
    const auto& player_status = engine::player->destructible;
    render_bar(position_t(1, 1), BAR_WIDTH, "HP", player_status->hp,
               player_status->max_hp, TCODColor::lightRed,
               TCODColor::darkerRed);
    render_mouse_look();
    render_messages();
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
    m_console.printf(
        pos.x + width / 2, pos.y, TCOD_bkgnd_flag_t::TCOD_BKGND_NONE,
        TCOD_alignment_t::TCOD_CENTER, "%s : %g/%g", name, value, max_value);
}

void Gui::render_mouse_look() {
    auto mouse_pos = position_t{engine::mouse.cx, engine::mouse.cy};
    if(!engine::map->is_in_fov(mouse_pos)) {
        return;
    }

    char buf[128] = {0};
    bool first    = true;
    for(auto& actor : engine::actors) {
        if(actor->position == mouse_pos) {
            if(!first) {
                strcat(buf, ", ");
            }
            else {
                first = false;
            }
            strcat(buf, actor->name);
        }
    }
    m_console.setDefaultForeground(TCODColor::lightGrey);
    m_console.printf(1, 3, "%s", buf);
}

// void Gui::message(const char* text, const TCODColor& color) {}