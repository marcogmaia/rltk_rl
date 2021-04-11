#pragma once

#include "entt/entity/registry.hpp"
// #include "libtcod.hpp"
#include "rltk/rltk.hpp"
#include "component/position.hpp"
#include "core/engine.hpp"



namespace radl {

/**
 * @return true
 * @return false
 */
engine::game_state_t player_input();

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

/**
 * @brief given an event, return the delta pos that would be occurred
 * 
 * @param ev 
 * @return position_t 
 */
position_t get_delta_pos(const sf::Event& ev);

}  // namespace radl
