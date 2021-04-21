#pragma once
#include <vector>

#include "entt/entity/registry.hpp"
#include "component/common.hpp"
#include "component/position.hpp"

namespace radl::component {

struct combat_stats_t {
    int max_hp;   // maximum health points
    int hp;       // current health points
    int defense;  // hit points deflected
    int power;    // atk power

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(max_hp, hp, defense, power);
    }
};

struct wants_to_melee_t {
    entt::entity target;
};

struct suffer_damage_t {
    std::vector<int> amount;
};

struct dead_t {
    int decompose_turns = 200;
};

inline void new_damage(registry& reg, entt::entity ent_target, int damage) {
    if(auto* suffering = reg.try_get<suffer_damage_t>(ent_target)) {
        suffering->amount.push_back(damage);
    } else {
        reg.emplace<suffer_damage_t>(ent_target).amount.push_back(damage);
    }
}


}  // namespace radl::component
