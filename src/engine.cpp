#include <iostream>
#include <algorithm>
#include <future>
#include <fmt/format.h>
#include <execution>
#include "engine.hpp"

namespace engine {

Actor* player;
std::vector<std::unique_ptr<Actor>> actors;
std::unique_ptr<Map> map;
game_status_t game_status = game_status_t::STARTUP;

void init() {
    static bool initialized = false;
    if(initialized) {
        return;
    }
    initialized = true;

    TCODConsole::initRoot(80, 50, "Maia Learning", false,
                          TCOD_renderer_t::TCOD_RENDERER_SDL2);
    TCODSystem::setFps(120);
    map = std::make_unique<Map>(80, 45);

    const auto& rooms = map->get_room_positions();
    if(rooms.size() > 0) {
        actors.emplace_back(
            std::make_unique<Actor>(rooms[0].random_pos(), '@', "", 8));
        player = actors.back().get();
        map->compute_fov(*player);
    }

    if(rooms.size() > 1) {
        std::for_each(std::cbegin(rooms) + 1, std::cend(rooms),
                      [](const rect_t& rect) {
                          map->add_enemy(rect.random_pos());
                      });
    }
}

bool get_next_position(const TCOD_key_t& key, position_t* pos) {
    bool performed_move = false;
    int dy              = 0;
    int dx              = 0;
    switch(key.vk) {
    case TCODK_KP8:
    case TCODK_UP: {
        --dy;
    } break;
    case TCODK_KP2:
    case TCODK_DOWN: {
        ++dy;
    } break;
    case TCODK_KP4:
    case TCODK_LEFT: {
        --dx;
    } break;
    case TCODK_KP6:
    case TCODK_RIGHT: {
        ++dx;
    } break;
    case TCODK_KP7: {
        --dx;
        --dy;
    } break;
    case TCODK_KP9: {
        ++dx;
        --dy;
    } break;
    case TCODK_KP1: {
        --dx;
        ++dy;
    } break;
    case TCODK_KP3: {
        ++dx;
        ++dy;
    } break;
    default:
        break;
    }
    pos->x += dx;
    pos->y += dy;
    if(dx != 0 || dy != 0) {
        performed_move = true;
    }
    return performed_move;
}

bool get_key_mouse_event(TCOD_key_t* keypress, TCOD_mouse_t* mouse) {
    TCOD_event_t ret = TCODSystem::checkForEvent(
        TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, keypress, mouse);
    return ret != TCOD_EVENT_NONE;
}

// nice candidate to parallel dispatch
static void update_enemies() {
    // using parallel algorithms
    // std::for_each(std::execution::par_unseq, actors.begin(), actors.end(),
    //               [](std::unique_ptr<Actor>& actor) {
    //                   if(actor.get() == player) {
    //                       return;
    //                   }
    //                   actor->update();
    //               });

    // using futures
    std::vector<std::future<void>> futures;
    for(auto& actor : actors) {
        futures.emplace_back(std::async(std::launch::async, [&actor]() {
            if(actor.get() == player) {
                return;
            }
            actor->update();
        }));
    }
    std::cout << "waiting updates...\n";
    for(auto& future : futures) {
        future.wait();
    }
    std::cout << fmt::format("done update. futures.size: {}\n", futures.size());

    // update all, then add to action_queue to perform the action in proper
    // order
}

static void handle_game_status() {
    switch(game_status) {
    case game_status_t::STARTUP: {
        map->compute_fov(*player);
        game_status = game_status_t::IDLE;
    } break;
    case game_status_t::IDLE: {
        TCOD_key_t keypress;
        TCOD_mouse_t mouse;
        get_key_mouse_event(&keypress, &mouse);
        auto player_pos     = player->position;
        position_t next_pos = player_pos;
        bool performed_move = get_next_position(keypress, &next_pos);
        if(performed_move) {
            player->move_attack(next_pos);
            map->compute_fov(*player);
            game_status = NEW_TURN;
        }
    } break;
    case game_status_t::NEW_TURN: {
        update_enemies();
        game_status = IDLE;
    } break;
    case game_status_t::DEFEAT: {
    } break;
    case game_status_t::VICTORY: {
    } break;
    default:
        break;
    }
}


void update() {
    if(player == nullptr) {
        return;
    }
    handle_game_status();
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
