#pragma once

#include "entt/entt.hpp"
#include "component/component.hpp"


namespace radl {

namespace engine::factory {

void player_factory(entt::entity ent, const position_t& pos,
                    const vchar_t& vch);

void enemy_factory(const position_t& pos, vchar_t vch, const char* name);

}  // namespace engine::factory

}  // namespace radl
