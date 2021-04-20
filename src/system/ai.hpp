#pragma once

#include "component/common.hpp"
#include "component/position.hpp"

namespace radl::system {

using namespace component;

// XXX unused
struct ai_t {};

void ai_enemy();

void ai_enemy_find_path(entity e_ent, const position_t& target_pos);

void ai_enemy_dijkstra_map(entity ent);

}  // namespace radl::system
