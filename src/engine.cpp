#include <iostream>
#include "engine.hpp"

namespace engine {

Actor* player;
std::vector<std::unique_ptr<Actor>> actors;
std::unique_ptr<Map> map;

void init() {
    static bool initialized = false;
    if(initialized) {
        return;
    }
    initialized = true;

    TCODConsole::initRoot(80, 80, "Maia Learning", false);
    TCODSystem::setFps(30);
    map = std::make_unique<Map>(80, 70);

    const auto& room_pos = map->get_room_positions();
    if(room_pos.size() > 0) {
        actors.emplace_back(
            std::make_unique<Actor>(room_pos[0].center(), '@', "", 8));
        player = actors.back().get();
        map->compute_fov(*player);
    }

    if(room_pos.size() > 1) {
        std::for_each(std::cbegin(room_pos) + 1, std::cend(room_pos),
                      [](const rect_t& rect) {
                          map->add_enemy(rect.random_pos());
                      });
    }
}

bool get_next_position(const TCOD_key_t& key, position_t* pos) {
    bool got_position = false;
    switch(key.vk) {
    case TCODK_KP8:
    case TCODK_UP: {
        got_position = true;
        --pos->y;
    } break;
    case TCODK_KP2:
    case TCODK_DOWN: {
        got_position = true;
        ++pos->y;
    } break;
    case TCODK_KP4:
    case TCODK_LEFT: {
        got_position = true;
        --pos->x;
    } break;
    case TCODK_KP6:
    case TCODK_RIGHT: {
        got_position = true;
        ++pos->x;
    } break;
    case TCODK_KP7: {
        got_position = true;
        --pos->x;
        --pos->y;
    } break;
    case TCODK_KP9: {
        got_position = true;
        ++pos->x;
        --pos->y;
    } break;
    case TCODK_KP1: {
        got_position = true;
        --pos->x;
        ++pos->y;
    } break;
    case TCODK_KP3: {
        got_position = true;
        ++pos->x;
        ++pos->y;
    } break;
    default:
        break;
    }
    return got_position;
}

bool has_event(TCOD_key_t* keypress, TCOD_mouse_t* mouse,
               uint32_t timeout = 20) {
    TCODSystem::getElapsedMilli();

    auto ret = TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE,
                                        keypress, mouse, false);
    return ret != TCOD_EVENT_NONE;
}


void update() {
    if(player == nullptr) {
        return;
    }
    TCOD_key_t keypress;
    TCOD_mouse_t mouse;

    has_event(&keypress, &mouse);
    auto& player_pos = player->position;
    position_t pos   = player_pos;
    get_next_position(keypress, &pos);
    if(map->is_walkable(pos)) {
        player_pos = pos;
        map->compute_fov(*player);
    }
}

void render() {
    TCODConsole::root->clear();
    map->render();
    for(const auto& actor : actors) {
        // only show actors in fov
        if(map->is_in_fov(actor->position)) {
            actor->render();
        }
    }
}

}  // namespace engine
