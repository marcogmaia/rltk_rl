#pragma once
#include <memory>
#include <deque>
#include <list>
#include "entity.hpp"
#include "map.hpp"
#include "gui.hpp"


namespace engine {

extern Entity* player;
extern std::vector<std::unique_ptr<Entity>> entities;
extern std::unique_ptr<Map> map;
extern TCOD_key_t last_key;
extern TCOD_mouse_t mouse;

extern uint32_t screen_width;
extern uint32_t screen_height;

extern Gui gui;

extern std::list<Entity*> entity_render_list;


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
