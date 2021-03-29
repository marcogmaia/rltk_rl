#pragma once

#include <memory>
#include <vector>

#include "rltk/rltk.hpp"

#include "utils/rect.hpp"
#include "component/vchar.hpp"

namespace radl::world {

enum tile_type_t {
    wall,
    floor,
};

using namespace rltk::colors;
struct tile_t {
    vchar_t vchar       = {'#', WHITE, BLACK};
    bool is_transparent = false;
    bool is_walkable    = false;
    bool is_explored    = false;
    // bool is_visible     = false;
    tile_type_t type    = wall;
};

struct Map {
    rect_t rect;
    std::vector<tile_t> tiles;
    std::vector<rect_t> rooms;

    inline const tile_t& operator[](position_t pos) const {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline tile_t& operator[](position_t pos) {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline tile_t& at(position_t pos) {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline tile_t& at(int x, int y) {
        return tiles[x + y * rect.width()];
    }

    inline const tile_t& at(int x, int y) const {
        return tiles[x + y * rect.width()];
    }
};

Map make_test_map(const rect_t& dimension, const position_t& player_pos);

Map new_map(const rect_t& rect);

}  // namespace radl::world
