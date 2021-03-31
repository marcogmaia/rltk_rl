#pragma once

#include "entt/entity/registry.hpp"
#include "component/position.hpp"

namespace radl {

/**
 * @brief
 *
 * @param reg registry
 * @param ent entity
 * @param delta_pos delta position
 * @return true
 * @return false
 */
bool move_attack(entt::registry& reg, entt::entity& ent,
                 const position_t& delta_pos);

}  // namespace radl
