#pragma once

#include "system/visibility.hpp"
#include "system/player_action.hpp"

namespace radl::system {

/**
 * @brief runs every system on the ECS
 *
 */
void systems_run();

void system_particle(double elapsed_time);

void systems_player();

void init_systems();

}  // namespace radl::system
