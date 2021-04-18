#pragma once

// #include "entt/entity/registry.hpp"
#include "entt/fwd.hpp"
// #include "libtcod.hpp"
#include "rltk/rltk.hpp"
#include "component/position.hpp"
#include "core/engine.hpp"
#include "core/game_state.hpp"


namespace radl {

/**
 * @return true
 * @return false
 */
game_state_t player_input();

/**
 * @brief move (to tile) or attack something on @a dst_pos
 *
 * @param reg registry
 * @param ent entity
 * @param dst_pos destination position to move/attack
 * @return true if moved, false otherwise
 */
bool move_wait_attack(entt::entity& ent, const position_t& dst_pos);


void random_walk(registry& reg, const entt::entity& ent,
                 const position_t& src_pos);

/**
 * @brief given an event, return the delta pos that would be occurred
 *
 * @param ev
 * @return position_t
 */
position_t get_delta_pos(const sf::Event& ev);

}  // namespace radl
