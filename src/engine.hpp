#pragma once
#include <memory>
#include <list>
#include "entt/entt.hpp"
#include "map.hpp"


class Engine {
    TCOD_key_t last_key;
    entt::registry reg;
    entt::entity player;
    entt::entity map;

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
