#include <iostream>
#include <fmt/format.h>

#include "ai.hpp"
#include "engine.hpp"

// how to make the Actor class be self contained?
// I want to remove the map other actors.
//
// No need, since this is a cpp file, they are already decoupled


bool move_attack(Actor* p_actor, position_t pos) {
    auto& map = engine::map;
    if(!map->can_walk(pos))
        return false;
    for(auto& actor : engine::actors) {
        if(p_actor == actor.get())
            continue;
        if(pos == actor->position) {
            std::cout << fmt::format(
                "The {} laughs at your punny attempt to attack him!\n",
                actor->name);
            return false;
        }
    }
    p_actor->position = pos;
    return true;
}


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
    if(!(map->can_walk(target_pos)))
        return false;
    // look for living actors to attack
    for(auto& actor : engine::actors) {
        if(owner == actor.get())
            continue;
        if(actor->destructible && actor->position == target_pos
           && !actor->destructible->is_dead()) {
            //    owner->attacker->attack(owner, actor);
            std::cout << fmt::format(
                "The {} laughs at your punny attempt to attack him!\n",
                actor->name);
            return false;
        }
    }
    owner->position = target_pos;
    return true;
}

bool AiPlayer::update(Actor* owner) {
    if(owner->destructible && owner->destructible->is_dead())
        return false;
    position_t dpos;
    auto got_pos = get_next_position(engine::last_key, &dpos);
    auto target_pos = owner->position + dpos;
    if(got_pos) {
        move_attack(owner, target_pos);
    }
    return got_pos;
}
