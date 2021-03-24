#pragma once

#include <memory>
#include <vector>
#include <string>
#include <common.hpp>
#include "libtcod.hpp"

class Gui {
private:
    struct Message {
        std::string text;
        TCODColor color;
        Message(const char* text, const TCODColor& color);
        ~Message() = default;
    };

    static constexpr int PANEL_HEIGHT = 7;
    static constexpr int BAR_WIDTH    = 20;
    static constexpr int MSG_X        = BAR_WIDTH + 2;
    static constexpr int MSG_HEIGHT   = PANEL_HEIGHT + 2;

    // std::unique_ptr<TCODConsole> m_console;
    TCODConsole m_console;
    std::vector<Message> m_log;

    void render_bar(position_t pos, int width, const char* name, float value,
                    float max_value, const TCODColor& bar_color,
                    const TCODColor& back_color);

public:
    Gui();
    ~Gui();
    void render();
    void message(const char* text, const TCODColor& color);
};
