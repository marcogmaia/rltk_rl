#pragma once
#include "component/component.hpp"

namespace radl {


struct attacker_t {
    float power = 0.F;
};

// void attack(const entity& ent_atk, const entity& ent_def);
void attack(const entity& ent_atk, const position_t& pos);

// inline void attack(entt::entity atkent, entt::entity defent) {
//     using engine::reg;
//     auto& attacker    = reg.get<attacker_t>(atkent);
//     auto defender     = reg.get<destructible_t>(defent);
//     auto final_damage = attacker.power - defender.defense;
//     if(final_damage < 0) {
//         final_damage = 0;
//     }
//     if(!defender.is_dead()) {
//         reg.patch<destructible_t>(defent, [&](destructible_t& def) {
//             def.hp -= final_damage;
//         });
//     }
// }


}  // namespace radl
