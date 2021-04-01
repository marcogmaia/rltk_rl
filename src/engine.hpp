#pragma once
#include <memory>
#include <list>
#include "entt/entt.hpp"
#include "map.hpp"

namespace radl {


namespace engine {

extern entt::observer observer;
extern entt::registry reg;
extern entt::entity player;
// extern world::Map map;
extern entt::entity map;

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

}  // namespace radl