#pragma once
#include <memory>
#include <deque>
#include <list>
#include "actor.hpp"
#include "map.hpp"


namespace engine {

extern Actor* player;
extern std::vector<std::unique_ptr<Actor>> actors;
extern std::unique_ptr<Map> map;
extern TCOD_key_t last_key;

extern std::list<Actor*> actor_render_list;

enum game_status_t {
    STARTUP,
    IDLE,
    NEW_TURN,
    VICTORY,
    DEFEAT,
};

void init();
void update();
void render();

}  // namespace engine
