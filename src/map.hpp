#pragma once

#include <memory>
#include <vector>

#include "libtcod.hpp"

#include "rect.hpp"

using namespace radl;

struct tile_t {
    bool is_transparent = false;
    bool is_walkable    = false;
    bool is_explored    = false;
};

struct Map {
    rect_t rect;
};
