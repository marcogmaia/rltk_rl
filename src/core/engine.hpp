#pragma once
#include <memory>
#include <queue>
#include <list>

#include "SFML/Window.hpp"

#include "entt/entity/fwd.hpp"

#include "system/map/map.hpp"

#include "entt/entity/fwd.hpp"
#include "entt/signal/dispatcher.hpp"

#include "core/gui/gui.hpp"

namespace radl {

using entt::entity;
using entt::registry;

}  // namespace radl


namespace radl::engine {


extern rltk::virtual_terminal* console;

using namespace component;

class Engine {
private:
    class EngineImpl;
    std::unique_ptr<EngineImpl> engine_impl;
    std::queue<sf::Event> event_queue;

    std::unique_ptr<gui::RadlUI> ui;

public:
    Engine();
    ~Engine();

    entt::dispatcher dispatcher;

    void set_kb_event(const sf::Event& event);
    void set_mouse_event(const sf::Event& event);

    /**
     * @brief Get the keyboard event
     *
     * @param event - event output
     * @return true if event was written to @a event
     */
    bool get_kb_event(sf::Event& event);

    sf::Event get_mouse_event();

    position_t get_mouse_position();

    void game_tick();
    void run_game();
};

extern Engine engine;

}  // namespace radl::engine

// namespace radl::state {

// bool is_window_focused();

// /* Setter function for window focus */
// void set_window_focus_state(const bool& s);

// /* Mouse state reset: clears all mouse state */
// void reset_mouse_state();

// /* Update the stored mouse position. Does not actually move the mouse. */
// void set_mouse_position(int x, int y);

// position_t get_mouse_position();

// position_t get_mouse_position_coord();

// /* Mouse button state */
// void set_mouse_button_state(int button, bool state);

// bool get_mouse_button_state(int button);

// /* Keyboard queue */
// // void enqueue_key_pressed(sf::Event& event);

// }  // namespace radl::state
