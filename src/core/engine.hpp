#pragma once
#include <memory>
#include <list>

#include "SFML/Window.hpp"

#include "entt/entt.hpp"
#include "core/map.hpp"
#include "component/log.hpp"

using entt::entity;
using entt::registry;

namespace radl::engine {

extern entt::registry reg;
extern entt::entity player;
extern std::deque<sf::Event> event_queue;
extern entt::sigh<sf::Event()> ev_signal;

extern rltk::virtual_terminal* console;

extern entt::dispatcher event_dispatcher;

using namespace component;

Map& get_map();
game_log_t& get_game_log();
position_t get_position_from_entity(entity ent);

enum game_state_t {
    PRE_RUN,
    AWAITING_INPUT,
    PLAYER_TURN,
    SHOW_INVENTORY,
    ENEMY_TURN,
    VICTORY,
    DEFEAT,
};

void init();
void update(double elapsed_time);
void render();
void terminate();

}  // namespace radl::engine

namespace radl::state {

bool is_window_focused();

/* Setter function for window focus */
void set_window_focus_state(const bool& s);

/* Mouse state reset: clears all mouse state */
void reset_mouse_state();

/* Update the stored mouse position. Does not actually move the mouse. */
void set_mouse_position(const int x, const int y);

position_t get_mouse_position();

/* Mouse button state */
void set_mouse_button_state(const int button, const bool state);
bool get_mouse_button_state(const int button);

/* Keyboard queue */
// void enqueue_key_pressed(sf::Event& event);


}  // namespace radl::state


using namespace radl::component;
using radl::engine::player;
using radl::engine::reg;
