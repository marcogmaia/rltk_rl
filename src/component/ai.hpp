#pragma once
#include "core/engine.hpp"
#include "spdlog/spdlog.h"

namespace radl {

struct ai_t {};

void ai_enemy();

void ai_enemy_find_path(entity e_ent, const position_t& target_pos);
}  // namespace radl
