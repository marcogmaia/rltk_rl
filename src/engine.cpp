#include "engine.hpp"
#include <iostream>

namespace engine {

namespace {

std::vector<std::unique_ptr<Actor>> actors;
std::unique_ptr<Actor> player;
std::unique_ptr<Map> map;

}  // namespace

// Engine engine;

void init() {
    static bool initialized = false;
    if(initialized) {
        return;
    }
    initialized = true;

    TCODConsole::initRoot(80, 50, "Maia Learning", false);
    TCODSystem::setFps(30);
    map = std::make_unique<Map>(80, 45);

    const auto& room_pos = map->get_room_positions();
    if(room_pos.size() > 0) {
        player = std::make_unique<Actor>(room_pos[0], '@', TCODColor::white);
    }

    if(room_pos.size() > 1) {
        std::for_each(std::cbegin(room_pos) + 1, std::cend(room_pos),
                      [](const position_t& pos) {
                          auto gen = TCODRandom::getInstance()->getInt(0, 1);
                          if(gen == 0) {
                              actors.emplace_back(std::make_unique<Actor>(
                                  pos, 'D', TCODColor::red));
                          }
                      });
    }
}

position_t get_next_position(const TCOD_key_t& key, position_t pos) {
    switch(key.vk) {
    case TCODK_KP8:
    case TCODK_UP: {
        --pos.y;
    } break;
    case TCODK_KP2:
    case TCODK_DOWN: {
        ++pos.y;
    } break;
    case TCODK_KP4:
    case TCODK_LEFT: {
        --pos.x;
    } break;
    case TCODK_KP6:
    case TCODK_RIGHT: {
        ++pos.x;
    } break;
    case TCODK_KP7: {
        --pos.x;
        --pos.y;
    } break;
    case TCODK_KP9: {
        ++pos.x;
        --pos.y;
    } break;
    case TCODK_KP1: {
        --pos.x;
        ++pos.y;
    } break;
    case TCODK_KP3: {
        ++pos.x;
        ++pos.y;
    } break;
    default:
        break;
    }
    return pos;
}

bool has_event(TCOD_key_t* keypress, TCOD_mouse_t* mouse,
               uint32_t timeout = 20) {
    TCODSystem::getElapsedMilli();

    auto ret = TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE,
                                        keypress, mouse, false);
    return ret != TCOD_EVENT_NONE;
}

void update() {
    TCOD_key_t keypress;
    TCOD_mouse_t mouse;
    // constexpr uint32_t timeout = 10;

    has_event(&keypress, &mouse);
    if(player) {
        auto& player_pos = player->position;

        auto pos = get_next_position(keypress, player_pos);
        if(map->is_walkable(pos)) {
            player_pos = pos;
        }
    }
}

void render() {
    TCODConsole::root->clear();
    map->render();
    for(const auto& actor : actors) {
        actor->render();
    }
    player->render();
}

}  // namespace engine
