#pragma once

#include "entt/entity/registry.hpp"
#include "position.hpp"

namespace radl {

bool move(entt::registry& reg, entt::entity& e, const position_t& dst_pos);

}
