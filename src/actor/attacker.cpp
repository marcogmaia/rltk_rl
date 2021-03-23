#include <iostream>
#include <fmt/format.h>
#include "actor.hpp"
#include "attacker.hpp"

Attacker::Attacker(float power)
    : power(power) {}

void Attacker::attack(Actor* owner, Actor* target) {
    if(target->destructible && !target->destructible->is_dead()) {
        auto damage = target->destructible->take_damage(target, power);
        if(damage > 0) {
            std::cout << fmt::format("{} attacks {} for {} hitpoints\n",
                                     owner->name, target->name, damage);
        }
        else {
            std::cout << fmt::format("{} attacks {}, but it has no effect.\n",
                                     owner->name, target->name);
        }
    }
    else {
        std::cout << fmt::format("{} attacks {} in vain.", owner->name,
                                 target->name);
    }
}
