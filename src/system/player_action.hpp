#pragma once

#include "entt/entity/registry.hpp"
// #include "libtcod.hpp"
#include "rltk/rltk.hpp"
#include "component/position.hpp"


namespace radl {

bool process_input(entt::registry&, entt::entity e);

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

void walk(const entt::entity& ent, const position_t& src_pos,
          const position_t& target_pos);

void random_walk(const entt::entity& ent, const position_t& src_pos);

}  // namespace radl
