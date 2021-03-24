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
            if(actor->position == target_pos) {
                std::cout << fmt::format("You attack the {}.\n", actor->name);
                return false;
            }
        }
        else {
            if(actor->position == target_pos) {
                std::cout << fmt::format(
                    "The {} laughs at your punny attempts to attack him.\n",
                    actor->name);
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
    auto dpos      = target - owner->position;
    float distance = dpos.normalize();
    if(distance >= 2.f) {
        dpos.x = static_cast<int>(roundf(dpos.x / distance));
        dpos.y = static_cast<int>(roundf(dpos.y / distance));
        if(engine::map->can_walk(owner->position + dpos)) {
            owner->position = owner->position + dpos;
        }
        else if(owner->attacker) {
            owner->attacker->attack(owner, engine::player);
        }
    }
}

bool AiEnemy::update(Actor* owner) {
    if(owner->is_dead()) {
        return false;
    }
    move_attack(owner, engine::player->position);

    return true;
}
