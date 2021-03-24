#include <iostream>
#include <fmt/format.h>
#include "actor.hpp"
#include "attacker.hpp"

Attacker::Attacker(float power)
    : power(power) {}

void Attacker::attack(Actor* owner, Actor* target) {
    if(target->destructible && !target->destructible->is_dead()) {
        std::cout << fmt::format("The {} attacks the {}.\n", owner->name,
                                 target->name);
        auto damage = target->destructible->take_damage(target, power);
    }
    else if(!target->destructible) {
        std::cout << fmt::format("{} attacks {} in vain.", owner->name,
                                 target->name);
    }
}
