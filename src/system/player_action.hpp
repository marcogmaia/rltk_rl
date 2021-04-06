#pragma once

#include "entt/entity/registry.hpp"
// #include "libtcod.hpp"
#include "rltk/rltk.hpp"
#include "component/position.hpp"


namespace radl {

/**
 * @brief eu não sei qual a definição dessa porra dessa função, 
 * 
 * @param ent
 * @return true 
 * @return false 
 */
bool process_input(entt::entity ent);

/**
 * @brief move (to tile) or attack something on @a dst_pos
 *
 * @param reg registry
 * @param ent entity
 * @param dst_pos destination position to move/attack
 * @return true if moved, false otherwise
 */
bool move_wait_attack(entt::entity& ent, const position_t& dst_pos);


void random_walk(const entt::entity& ent, const position_t& src_pos);

}  // namespace radl
