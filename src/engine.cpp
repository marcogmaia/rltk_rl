#include <iostream>
#include <algorithm>
#include <future>
#include <fmt/format.h>
#include <execution>
#include "engine.hpp"

namespace engine {

Actor* player;
std::vector<std::unique_ptr<Actor>> actors;

// TODO manage the actors render order
std::list<Actor*> actor_render_list;

std::unique_ptr<Map> map;
game_status_t game_status = game_status_t::STARTUP;
TCOD_key_t last_key;
uint32_t screen_width  = 80;
uint32_t screen_height = 50;

void init() {
    static bool initialized = false;
    if(initialized) {
        return;
    }
    initialized = true;

    TCODConsole::initRoot(screen_width, screen_height, "Maia Learning", false,
                          TCOD_renderer_t::TCOD_RENDERER_SDL2);
    TCODSystem::setFps(120);
    // init_game_map();
}

void init_game_map() {
    actors.clear();
    map.reset();
    map = std::make_unique<Map>(80, 45);

    const auto& rooms = map->get_rooms();
    if(!rooms.empty()) {
        auto pos = rooms[0].random_pos();
        while(!map->can_walk(pos)) {
            pos = rooms[0].random_pos();
        }
        auto uptr_player = std::make_unique<Actor>(pos, '@', "Player", 8);
        player           = uptr_player.get();
        player->ai       = std::make_unique<AiPlayer>();
        player->attacker = std::make_unique<Attacker>(5.f);
        player->destructible
            = std::make_unique<DestructiblePlayer>(100.f, 2.f, "ded");
        actors.emplace_back(std::move(uptr_player));
        map->compute_fov(*player);
    }
}

void terminate() {
    // actors.clear();
    // map.reset();
}

bool get_key_mouse_event(TCOD_key_t* keypress) {
    TCOD_event_t ret
        = TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, keypress, nullptr);
    return ret != TCOD_EVENT_NONE;
}

// nice candidate to parallel dispatch
static void update_enemies() {
    // using parallel algorithms
    std::for_each(std::execution::par_unseq, actors.begin(), actors.end(),
                  [](std::unique_ptr<Actor>& actor) {
                      if(actor.get() == player) {
                          return;
                      }
                      actor->update();
                  });

    // using futures
    // std::vector<std::future<void>> futures;
    // for(auto& actor : actors) {
    //     futures.emplace_back(std::async(std::launch::async, [&actor]() {
    //         if(actor.get() == player) {
    //             return;
    //         }
    //         actor->update();
    //     }));
    // }
    // std::cout << "waiting updates...\n";
    // for(auto& future : futures) {
    //     future.wait();
    // }
    // std::cout << fmt::format("done update. futures.size: {}\n",
    // futures.size());

    // update all, then add to action_queue to perform the action in proper
    // order
}

static void handle_game_status() {
    switch(game_status) {
    case game_status_t::STARTUP: {
        init_game_map();
        map->compute_fov(*player);
        game_status = game_status_t::IDLE;
    } break;
    case game_status_t::IDLE: {
        get_key_mouse_event(&last_key);
        if(player->is_dead()) {
            game_status = game_status_t::DEFEAT;
        }
        if(player->update()) {
            map->compute_fov(*player);
            game_status = NEW_TURN;
        }
    } break;
    case game_status_t::NEW_TURN: {
        update_enemies();
        game_status = IDLE;
    } break;
    case game_status_t::DEFEAT: {
        terminate();
        game_status = STARTUP;
    } break;
    case game_status_t::VICTORY: {
    } break;
    default:
        break;
    }
}


void update() {
    // if(player == nullptr) {
    //     return;
    // }
    handle_game_status();
}

void render() {
    TCODConsole::root->clear();
    map->render();
    // render_actors();
    for(const auto& actor : actors) {
        // only show actors in fov
        if(map->is_in_fov(actor->position)) {
            actor->render();
        }
    }
    player->render();
    // show player stats
    TCODConsole::root->print(1, screen_height - 2, "HP : %d/%d",
                             static_cast<int>(player->destructible->hp),
                             static_cast<int>(player->destructible->max_hp));
}

}  // namespace engine
