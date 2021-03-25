#pragma once

#include <fmt/format.h>
#include <memory>
#include <list>
#include <string>
#include <common.hpp>
#include "libtcod.hpp"

class Gui {
private:
    struct Message {
        std::string text;
        TCODColor color;
        Message(const char* text, const TCODColor& color);
        Message(const std::string& text, const TCODColor& color);
        Message(const std::string&& text, const TCODColor& color);
        Message(const Message&) = default;
        Message(Message&&)      = default;
        ~Message()              = default;
    };

    static constexpr int PANEL_HEIGHT = 7;
    static constexpr int BAR_WIDTH    = 20;
    static constexpr int MSG_X        = BAR_WIDTH + 2;
    static constexpr int MSG_HEIGHT   = PANEL_HEIGHT;

    // std::unique_ptr<TCODConsole> m_console;
    TCODConsole m_console;
    std::list<Message> m_log;

    void render_bar(position_t pos, int width, const char* name, float value,
                    float max_value, const TCODColor& bar_color,
                    const TCODColor& back_color);

    void render_mouse_look();

    void render_messages();

public:
    Gui() noexcept;
    ~Gui();
    void render();

    /**
     * @brief
     *
     * @tparam Args
     * @param color
     * @param fmt
     * @param args
     */
    template <typename S, typename... Args>
    void message(const TCODColor& color, const S& fmt, Args&&... args) {
        Message msg = {
            .text  = fmt::format(fmt, args...),
            .color = color,
        };
        m_log.emplace_back(std::move(msg));
        if(m_log.size() > MSG_HEIGHT) {
            m_log.pop_front();
        }
    }
    // void message(const char* text, const TCODColor& color);
};
