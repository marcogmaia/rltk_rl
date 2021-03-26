#pragma once
#include <memory>
#include <list>
#include "entt/entt.hpp"
#include "map.hpp"


class Engine {
    entt::registry reg;
    entt::entity player;


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
