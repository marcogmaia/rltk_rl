#pragma once
#include <memory>
#include <list>

#include "SFML/Window.hpp"

#include "entt/entity/fwd.hpp"

#include "system/map/map.hpp"

#include "entt/entity/fwd.hpp"
#include "entt/signal/dispatcher.hpp"

namespace radl {

using entt::entity;
using entt::registry;

}  // namespace radl


namespace radl::engine {

extern std::deque<sf::Event> event_queue;
extern entt::sigh<sf::Event()> ev_signal;

extern rltk::virtual_terminal* console;

extern entt::dispatcher event_dispatcher;

using namespace component;


// TODO PImpl to create a "compiler firewall" between main and systems
class Engine {
private:
    class EngineImpl;
    std::unique_ptr<EngineImpl> engine_impl;

public:
    Engine();
    ~Engine();

    entt::dispatcher dispatcher;

    void init();
    void terminate();
    void set_kb_event(const sf::Event& event);
    void set_mouse_event(const sf::Event& event);
    sf::Event get_kb_event();
    sf::Event get_mouse_event();
    void game_tick();
    void run_game();
};

void init();
void update(double elapsed_time);
void terminate();

}  // namespace radl::engine

namespace radl::state {

bool is_window_focused();

/* Setter function for window focus */
void set_window_focus_state(const bool& s);

/* Mouse state reset: clears all mouse state */
void reset_mouse_state();

/* Update the stored mouse position. Does not actually move the mouse. */
void set_mouse_position(int x, int y);

position_t get_mouse_position();

position_t get_mouse_position_coord();

/* Mouse button state */
void set_mouse_button_state(int button, bool state);

bool get_mouse_button_state(int button);

/* Keyboard queue */
// void enqueue_key_pressed(sf::Event& event);


}  // namespace radl::state
