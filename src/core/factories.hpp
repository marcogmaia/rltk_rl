#pragma once

#include "entt/entt.hpp"
// #include "attacker.hpp"
#include "component/component.hpp"


namespace radl {

namespace engine::factory {

void player_factory(entt::entity ent, const position_t& pos,
                    const vchar_t& vch);

void enemy_factory(const position_t& pos, vchar_t vch);

}  // namespace engine::factory

}  // namespace radl
