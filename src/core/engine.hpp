#pragma once
#include <memory>
#include <list>
#include "entt/entt.hpp"
#include "core/map.hpp"

using entt::entity;
using entt::registry;

namespace radl::engine {

// extern entt::observer observer;
extern entt::registry reg;
extern entt::entity player;
extern std::queue<sf::Event> event_queue;

extern rltk::virtual_terminal* console;
// extern sf::RenderWindow *main_window;

world::Map& get_map();

enum game_state_t {
    STARTUP,
    PLAYER_TURN,
    ENEMY_TURN,
    VICTORY,
    DEFEAT,
};

void init();
void update();
void render();
void terminate();

}  // namespace radl::engine
