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

world::Map& get_map();

enum game_state_t {
    STARTUP,
    IDLE,
    NEW_TURN,
    VICTORY,
    DEFEAT,
};

void init();
void update();
// void render();
void terminate();

}  // namespace radl::engine
