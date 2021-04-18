#pragma once

#include "component/position.hpp"

namespace radl::component {

struct enemy_t {
    bool remembers_target_position = false;
    int remember_turns = 0;
    position_t memory;
};

}  // namespace radl
