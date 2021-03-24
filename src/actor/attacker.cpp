#include <iostream>
#include <fmt/format.h>
#include "actor.hpp"
#include "attacker.hpp"
#include "engine.hpp"

Attacker::Attacker(float power)
    : power(power) {}

void Attacker::attack(Actor* owner, Actor* target) {
    if(target->destructible && !target->destructible->is_dead()) {
        if(owner == engine::player) {
            engine::gui.message(TCODColor::blue, "You attack the {}.\n",
                                target->name);
        }
        else if(target == engine::player) {
            engine::gui.message(TCODColor::darkRed, "{} attacks you.\n",
                                owner->name);
        }
        else {
            engine::gui.message(TCODColor::lightRed, "{} attacks {}.\n",
                                owner->name, target->name);
        }
        target->destructible->take_damage(target, power);
    }
    else if(!target->destructible) {
        engine::gui.message(TCODColor::yellow, "{} attacks {} in vain.\n",
                            owner->name, target->name);
    }
}
