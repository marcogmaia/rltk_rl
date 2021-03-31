#pragma once
#include <memory>
#include <list>
#include "entt/entt.hpp"
#include "map.hpp"

namespace radl {

extern entt::registry reg;
extern entt::entity player;
// extern world::Map map;
extern entt::entity map;


extern entt::observer observer;

class Engine {

    enum game_status_t {
        STARTUP,
        IDLE,
        NEW_TURN,
        VICTORY,
        DEFEAT,
    };

public:
    void init();
    void update();
    // void render();
};

}  // namespace radl