#include <iostream>
#include <fmt/format.h>

#include "ai.hpp"
#include "engine.hpp"

// how to make the Actor class be self contained?
// I want to remove the map other actors.
//
// No need, since this is a cpp file, they are already decoupled

static bool get_next_position(const TCOD_key_t& key, position_t* pos) {
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


bool AiPlayer::move_attack(Actor* owner, position_t target_pos) {
    auto& map = engine::map;
    if(map->is_wall(target_pos))
        return false;
    // look for living actors to attack
    for(auto& actor : engine::actors) {
        if(owner == actor.get() || actor->is_dead()) {
            continue;
        }
        if(actor->destructible) {
            if(owner->attacker && actor->position == target_pos) {
                owner->attacker->attack(owner, actor.get());
                return false;
            }
        }
    }
    owner->position = target_pos;
    return true;
}

bool AiPlayer::update(Actor* owner) {
    if(owner->is_dead())
        return false;
    position_t dpos;
    auto got_pos    = get_next_position(engine::last_key, &dpos);
    auto target_pos = owner->position + dpos;
    if(got_pos) {
        move_attack(owner, target_pos);
    }
    return got_pos;
}

bool AiEnemy::move_attack(Actor* owner, position_t target) {
    auto moved     = true;
    auto dpos      = target - owner->position;
    auto stepx     = dpos.x > 0 ? 1 : -1;
    auto stepy     = dpos.y > 0 ? 1 : -1;
    float distance = dpos.normalize();
    if(distance >= 2.f) {
        dpos.x = static_cast<int>(roundf(dpos.x / distance));
        dpos.y = static_cast<int>(roundf(dpos.y / distance));
        if(engine::map->can_walk(owner->position + dpos)) {
            owner->position = owner->position + dpos;
        }
        else if(engine::map->can_walk(owner->position + position_t{stepx, 0})) {
            owner->position.x += stepx;
        }
        else if(engine::map->can_walk(owner->position + position_t{0, stepy})) {
            owner->position.y += stepy;
        }
    }
    else if(owner->attacker) {
        std::cout << fmt::format("The {} attacks you.\n", owner->name);
        owner->attacker->attack(owner, engine::player);
        moved = false;
    }
    return moved;
}

bool AiEnemy::update(Actor* owner) {
    if(owner->is_dead()) {
        return false;
    }

    if(engine::map->is_in_fov(owner->position)) {
        move_count = TRACKING_TURNS;
    }

    if(move_count > 0) {
        move_attack(owner, engine::player->position);
        --move_count;
    }
    return true;
}
