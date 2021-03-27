#pragma once
#include <memory>
#include <list>
#include "entt/entt.hpp"
#include "map.hpp"

namespace radl {

extern entt::registry reg;
extern entt::entity player;
extern entt::entity map;

class Engine {
    TCOD_key_t last_key;

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
    void render();
};

}  // namespace radl