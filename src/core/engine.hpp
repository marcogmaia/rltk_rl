#pragma once
#include <memory>
#include <list>
#include "entt/entt.hpp"
#include "core/map.hpp"

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

}  // namespace engine
