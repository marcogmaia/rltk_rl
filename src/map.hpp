#pragma once

#include <memory>
#include <vector>

#include "libtcod.hpp"

#include "component/rect.hpp"
#include "component/vchar.hpp"

namespace radl {


enum tile_type_t {
    wall,
    floor,
};

struct tile_t {
    bool is_transparent = false;
    bool is_walkable    = false;
    bool is_explored    = false;
    tile_type_t type    = wall;
};

struct Map {
    rect_t dimensions;
    std::vector<tile_t> tiles;

    Map(const rect_t& dimension);

    void fill(const rect_t& rect, const tile_t& tile);

    inline const tile_t& operator[](position_t pos) const {
        return tiles[pos.x + pos.y * dimensions.w];
    }
};


}  // namespace radl
