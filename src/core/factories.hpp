#pragma once

#include "entt/entt.hpp"
#include "component/component.hpp"


namespace radl {

namespace engine::factory {

void player_factory(entt::entity ent, const position_t& pos,
                    const vchar_t& vch);

entity enemy_factory(const position_t& pos, vchar_t vch, const char* name);

entity item_factory(const char* item_name, item_t item, vchar_t vch);

}  // namespace engine::factory

}  // namespace radl
