#pragma once
#include <memory>
#include <list>
#include "entt/entt.hpp"
#include "core/map.hpp"

using entt::registry;
using entt::entity;

namespace radl::engine {

// extern entt::observer observer;
extern entt::registry reg;
extern entt::entity player;
extern std::queue<sf::Event> event_queue;

enum game_status_t {
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

}  // namespace engine
