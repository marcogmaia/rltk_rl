#pragma once

#include "component/common.hpp"
#include "component/position.hpp"

namespace radl::component {

struct movable_t {};

struct want_to_walk_t {
    position_t from;
    position_t to;
};

void walk(registry& reg, const entt::entity& ent, const position_t& src_pos,
          const position_t& target_pos);


}  // namespace radl::component
