
#pragma once

#include "utils/rect.hpp"

namespace radl {

/**
 * @brief Spawn an enemy inside the delimited rect
 *
 * @param rect
 */
void enemy_spawner(component::rect_t rect);

void add_enemies();

}  // namespace radl
