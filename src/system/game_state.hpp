#pragma once

#include "entt/entity/registry.hpp"
#include "system/map/map.hpp"
#include "system/log.hpp"

namespace radl {

using entt::entity;
using entt::registry;

extern registry reg;
extern entity player;

enum game_state_t {
    PRE_RUN,
    AWAITING_INPUT,
    PLAYER_TURN,
    SHOW_INVENTORY,
    SHOW_INVENTORY_DROP,
    ENEMY_TURN,
    VICTORY,
    DEFEAT,
};

void game_state_init();


/**
 * @brief Get the map from the register context
 *
 * @warning call this function only after setting the map on the register
 *
 * @return the map from the register context
 */
inline Map& get_map() {
    return reg.ctx<Map>();
}


inline game_log_t& get_game_log() {
    return reg.ctx<game_log_t>();
}

position_t get_position_from_entity(entity ent);


}  // namespace radl
