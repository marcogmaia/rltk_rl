#include "engine.hpp"
#include <iostream>

namespace engine {

Engine engine;

Engine::Engine() {
    TCODConsole::initRoot(80, 50, "Maia Learning", false);
    TCODSystem::setFps(30);
    player = std::make_unique<Actor>(position_t{40, 25}, '@', TCODColor::white);
    actors.emplace_back(
        std::make_unique<Actor>(position_t{60, 13}, 'F', TCODColor::yellow));
    map = std::make_unique<Map>(80, 45);
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

void Engine::update() {
    TCOD_key_t keypress;
    TCOD_mouse_t mouse;
    TCODSystem::waitForEvent(TCOD_EVENT_ANY, &keypress, &mouse, false);
    auto& player_pos = player->position;
    auto pos         = get_next_position(keypress, player_pos);
    if(map->is_walkable(pos)) {
        player_pos = pos;
    }
#ifdef DEBUG
    std::cout << mouse.cx << ' ' << mouse.cy << std::endl;
#endif
}

void Engine::render() const {
    TCODConsole::root->clear();
    map->render();
    player->render();
    for(const auto& actor : actors) {
        actor->render();
    }
}

}  // namespace engine
