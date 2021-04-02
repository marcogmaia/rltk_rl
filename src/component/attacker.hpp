#pragma once
#include "component/component.hpp"

namespace radl {


struct attacker_t {
    float power = 0.F;
};

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
