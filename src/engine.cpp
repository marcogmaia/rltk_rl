#include <iostream>
#include <algorithm>
#include <future>
#include <fmt/format.h>
#include <execution>
#include "engine.hpp"

namespace engine {

Entity* player;
std::vector<std::unique_ptr<Entity>> entities;

// TODO manage the actors render order
std::list<Entity*> entity_render_list;

std::unique_ptr<Map> map;
game_status_t game_status = game_status_t::STARTUP;
TCOD_key_t last_key;
TCOD_mouse_t mouse;
uint32_t screen_width  = 100;
uint32_t screen_height = 60;

Gui gui;

static void init_charset() {
    // auto constexpr* font_file = "../assets/tiles.png";
    // auto constexpr* font_file = "../assets/terminal12x12_gs_ro.png";
    auto constexpr* font_file = "../assets/terminal16x16_gs_ro.png";
    // auto* tileset
    //     = TCOD_tileset_load(font_file, 16, 24, 256, TCOD_CHARMAP_CP437);
    TCOD_console_set_custom_font(
        font_file, TCOD_FONT_LAYOUT_CP437 | TCOD_FONT_TYPE_GRAYSCALE, 16, 16);
    TCOD_console_map_ascii_code_to_font('.', 10, 15);
    // TCOD_set_default_tileset(tileset);
    // TCOD_console_map_ascii_codes_to_font('A', 23, 1, 3);
}

void init() {
    static bool initialized = false;
    if(initialized) {
        return;
    }
    initialized = true;

    init_charset();

    TCODConsole::initRoot(screen_width, screen_height, "Maia Learning", false,
                          TCOD_renderer_t::TCOD_RENDERER_SDL2);
    auto& console = *TCODConsole::root;
    console.setDefaultBackground(TCODColor::darkestGrey);
    // console.
    TCODSystem::setFps(120);
    // init_game_map();
}

void init_game_map() {
    entities.clear();
    map.reset();
    map = std::make_unique<Map>(screen_width, screen_height - 7);

    const auto& rooms = map->get_rooms();
    if(!rooms.empty()) {
        auto pos = rooms[0].random_pos();
        while(!map->can_walk(pos)) {
            pos = rooms[0].random_pos();
        }
        auto uptr_player = std::make_unique<Entity>(pos, '@', "Player", 8);
        player           = uptr_player.get();
        player->ai       = std::make_unique<AiPlayer>();
        player->attacker = std::make_unique<Attacker>(5.F);
        player->destructible
            = std::make_unique<DestructiblePlayer>(100.F, 2.F, "ded");
        entities.emplace_back(std::move(uptr_player));
        map->compute_fov(*player);
    }
}

void terminate() {
    // actors.clear();
    // map.reset();
    // gui.clear()
}

bool get_key_mouse_event(TCOD_key_t* keypress, TCOD_mouse_t* mouse) {
    TCOD_event_t ret = TCODSystem::checkForEvent(
        TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, keypress, mouse);
    return ret != TCOD_EVENT_NONE;
}

// nice candidate to parallel dispatch
static void update_enemies() {
    // using parallel algorithms
    auto update_entity = [](std::unique_ptr<Entity>& entity) {
        if(entity.get() != player) {
            entity->update();
        }
    };
    std::for_each(entities.begin(), entities.end(), update_entity);

    // TODO update all, then add to action_queue to perform the action in proper
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
        get_key_mouse_event(&last_key, &mouse);
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

static void entities_render() {
    for(const auto& entity : entities) {
        // only show entity in fov
        if(map->is_in_fov(entity->position)) {
            entity->render();
        }
    }
}

void render() {
    TCODConsole::root->clear();
    map->render();
    entities_render();
    player->render();
    // show player stats
    gui.render();
}

}  // namespace engine
