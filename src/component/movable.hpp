#pragma once
#include "component/component.hpp"

namespace radl {


struct movable_t {};

struct want_to_walk_t {
    position_t from;
    position_t to;
};

void walk(const entt::entity& ent, const position_t& src_pos,
          const position_t& target_pos);


}  // namespace radl
