#pragma once

#include "entt/entt.hpp"
// #include "attacker.hpp"
#include "component/component.hpp"


namespace radl {

void player_factory(entt::registry& reg, entt::entity ent,
                    const position_t& pos, const radl::vchar_t& vch);
// entt::entity make_enemy(entt::registry& reg, const entt::entity& ent) {
//     entt::entity e = reg.create();
//     // TODO factories
//     reg.emplace<>(ent)
//         return ent;
// }


}  // namespace radl