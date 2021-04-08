#pragma once

namespace radl {

struct enemy_t {
    bool remembers_target_position = false;
    int remember_turns = 0;
    position_t memory;
};

struct being_t {
    const char* name;
};

}  // namespace radl
