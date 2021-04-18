#pragma once

#include "entt/entity/registry.hpp"

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

}  // namespace radl
