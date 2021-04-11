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

// extern entt::observer observer;
extern entt::registry reg;
extern entt::entity player;
extern std::deque<sf::Event> event_queue;
extern entt::sigh<sf::Event()> ev_signal;

extern rltk::virtual_terminal* console;

extern entt::dispatcher event_dispatcher;

world::Map& get_map();
component::game_log_t& get_game_log();

enum game_state_t {
    PRE_RUN,
    AWAITING_INPUT,
    PLAYER_TURN,
    ENEMY_TURN,
    VICTORY,
    DEFEAT,
};

void init();
void update(double elapsed_time);
void render();
void terminate();

}  // namespace radl::engine

using radl::engine::reg;
