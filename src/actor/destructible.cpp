#include "destructible.hpp"
#include "actor.hpp"

Destructible::Destructible(float max_hp, float defense, const char* corpse_name)
    : maxHp(maxHp)
    , defense(defense)
    , corpse_name(corpse_name) {}

float Destructible::take_damage(Actor* owner, float damage) {
    auto final_damage = damage - defense;
    if(final_damage < 0.f) {
        final_damage = 0.f;
    }
    hp -= final_damage;
    if(hp <= 0.f) {
        die(owner);
    }
    return final_damage;
}

void Destructible::die(Actor* owner) {}

DestructibleObject::DestructibleObject(float max_hp, float defense,
                                       const char* corpse_name)
    : Destructible(max_hp, defense, corpse_name) {}

DestructiblePlayer::DestructiblePlayer(float max_hp, float defense,
                                       const char* corpse_name)
    : Destructible(max_hp, defense, corpse_name) {}
