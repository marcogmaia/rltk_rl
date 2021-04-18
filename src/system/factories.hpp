#pragma once

#include "entt/entity/fwd.hpp"
#include "component/component.hpp"


namespace radl::factory {

// namespace factory {

void player_factory(entt::entity ent, const position_t& pos,
                    const vchar_t& vch);

entity enemy_factory(const position_t& pos, vchar_t vch, const name_t& name);

entity item_factory(const char* item_name, item_t item, vchar_t vch);


namespace items {

entity potion_healing(bool in_pack = false);

}


}  // namespace radl::factory
