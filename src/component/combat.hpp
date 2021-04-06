#pragma once
#include <vector>

#include <core/engine.hpp>

// #include <component/component.hpp>

namespace radl {

struct combat_stats_t {
    int max_hp;   // maximum health points
    int hp;       // current health points
    int defense;  // hit points deflected
    int power;    // atk power
};

struct wants_to_melee_t {
    entt::entity target;
};

struct suffer_damage_t {
    std::vector<int> amount;
};

struct dead_t {};

inline void new_damage(entt::entity ent_target, int damage) {
    using engine::reg;
    
    if(auto suffering = reg.try_get<suffer_damage_t>(ent_target)) {
        suffering->amount.push_back(damage);
    }
    else {
        reg.emplace<suffer_damage_t>(ent_target).amount.push_back(damage);
    }
}

void attack(const entity& ent_atk, const entity& ent_def);

void attack(const entity& ent_atk, const position_t& pos);


}  // namespace radl
