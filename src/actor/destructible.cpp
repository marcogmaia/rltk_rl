#include <iostream>
#include <fmt/format.h>
#include "destructible.hpp"
#include "actor.hpp"
#include "engine.hpp"

Destructible::Destructible(float max_hp, float defense, const char* corpse_name)
    : max_hp(max_hp)
    , hp(max_hp)
    , defense(defense)
    , corpse_name(corpse_name) {}

float Destructible::take_damage(Actor* owner, float damage) {
    auto final_damage = damage - defense;
    if(final_damage < 0.f) {
        final_damage = 0.f;
    }
    hp -= final_damage;
    std::cout << fmt::format("{} takes {} damage.\n", owner->name,
                             static_cast<int>(final_damage));
    if(hp <= 0.f) {
        die(owner);
    }
    return final_damage;
}

void Destructible::die(Actor* owner) {
    std::cout << fmt::format("{} dies.\n", owner->name);
    owner->ch     = '%';
    owner->color  = TCODColor::darkRed;
    owner->name   = corpse_name;
    owner->blocks = false;

    // make sure the corpse is sent to the back of the rendering queue
}

DestructibleEnemy::DestructibleEnemy(float max_hp, float defense,
                                     const char* corpse_name)
    : Destructible(max_hp, defense, corpse_name) {}

void DestructibleEnemy::die(Actor* owner) {
    Destructible::die(owner);
}

DestructiblePlayer::DestructiblePlayer(float max_hp, float defense,
                                       const char* corpse_name)
    : Destructible(max_hp, defense, corpse_name) {}

void DestructiblePlayer::die(Actor* owner) {
    // TODO death message
    std::cout << fmt::format(
        "The light slowly fades away as {} bleeds to the death... Now laying "
        "on the ground is {} motionless.\n",
        owner->name, owner->name);
    Destructible::die(owner);
}
