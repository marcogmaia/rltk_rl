#pragma once
#include "spdlog/spdlog.h"

#include "component/common.hpp"
#include "component/position.hpp"

namespace radl::component {

struct ai_t {};

void ai_enemy();

void ai_enemy_find_path(entity e_ent, const position_t& target_pos);

}  // namespace radl::component
