#pragma once

#include <memory>
#include <vector>

#include "libtcod.hpp"

#include "component/rect.hpp"
#include "component/vchar.hpp"

namespace radl::world {

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
    rect_t rect;
    std::vector<tile_t> tiles;
    std::vector<rect_t> rooms;

    inline const tile_t& operator[](position_t pos) const {
        return tiles[pos.x + pos.y * rect.width()];
    }

    inline tile_t& at(position_t pos) {
        return tiles[pos.x + pos.y * rect.width()];
    }

    inline tile_t& at(int x, int y) {
        return tiles[x + y * rect.width()];
    }

    inline const tile_t& at(int x, int y) const {
        return tiles[x + y * rect.width()];
    }
};

Map make_test_map(const rect_t& dimension, const position_t& player_pos);
Map new_map(const rect_t& rect, const position_t& player_pos);
}  // namespace radl::world
