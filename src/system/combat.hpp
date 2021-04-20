
#pragma once
#include "component/common.hpp"
#include "component/position.hpp"

namespace radl::system {

/**
 * @brief perform the attack on the defender
 *
 * @param ent_atk attacking entity
 * @param ent_def defender entity
 */
void attack(const entity& ent_atk, const entity& ent_def);

/**
 * @brief try an attack at the position
 *
 * @param ent_atk attacking entity
 * @param pos position which to try to attack
 */
void attack(const entity& ent_atk, const component::position_t& pos);


}  // namespace radl::system
