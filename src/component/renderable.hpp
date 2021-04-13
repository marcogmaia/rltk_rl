#pragma once
#include <string>
#include "rltk/rltk.hpp"
#include "vchar.hpp"

// To render anything we need to know the position, colors and console
// the position we get directly from the position_t struct of the entity

namespace radl::component {

enum z_level_t {
    GROUND = 0,
    DEAD,
    ITEM,
    BEING,
    MAX,
};

struct renderable_t {
    vchar_t vchar;
    z_level_t z_level = GROUND;
};

struct name_t {
    std::string name;
};

}  // namespace radl::component
