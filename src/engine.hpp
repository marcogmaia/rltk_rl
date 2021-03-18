#pragma once
#include <memory>
#include "actor.hpp"
#include "map.hpp"


namespace engine {

// class Engine {
// public:
    // std::vector<std::unique_ptr<Actor>> actors;
    // std::unique_ptr<Actor> player;
    // std::unique_ptr<Map> map;

    void init();
    void update();
    void render();
// };

// extern Engine engine;
}  // namespace engine
