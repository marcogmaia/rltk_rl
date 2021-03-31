#pragma once

#include <memory>
#include <vector>
#include <set>

#include "rltk/rltk.hpp"

#include "utils/rect.hpp"
#include "component/vchar.hpp"
#include "entt/entity/registry.hpp"

namespace radl::world {

using namespace rltk::colors;

enum tile_type_t {
    wall,
    floor,
};

struct tile_t {
    tile_type_t type;
};

// struct tile_t {
//     bool is_transparent = false;
//     bool is_walkable    = false;
//     bool is_explored    = false;
//     // bool is_visible     = false;
//     tile_type_t type = wall;
//     // vchar_t vchar    = {'#', WHITE, BLACK};
//     // cada tile tem uma lista de entidades
//     // std::vector<entt::entity> entities;

//     inline vchar_t get_vch() {
//         vchar_t vchar;
//         vchar.foreground = WHITE;
//         vchar.background = BLACK;
//         switch(type) {
//         case tile_type_t::floor: {
//             vchar.glyph = glyph::BLOCK1;
//         } break;
//         case tile_type_t::wall: {
//             vchar.glyph = glyph::SOLID1;
//         } break;
//         }
//         return vchar;
//     }
// };

struct visible_t {};

struct explored_t {};

struct tile_characteristics_t {
    bool walkable    = false;
    bool transparent = false;
};

struct Map {
    rect_t rect;

    std::vector<rect_t> rooms;

    entt::registry reg;

    std::vector<entt::entity> tiles;

    inline const entt::entity& operator[](position_t pos) const {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline entt::entity& operator[](position_t pos) {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline entt::entity& at(position_t pos) {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline entt::entity& at(int x, int y) {
        return tiles[x + y * rect.width()];
    }

    inline const entt::entity& at(int x, int y) const {
        return tiles[x + y * rect.width()];
    }

    void init(const rect_t& r) {
        rect      = r;
        auto area = r.area();
        tiles.resize(area);

        for(int x = 0; x < r.width(); ++x) {
            for(int y = 0; y < r.height(); ++y) {
                auto tile                = this->reg.create();
                tiles[x + y * r.width()] = tile;
                this->reg.emplace<position_t>(tile, position_t{x, y});
                this->reg.emplace<tile_t>(tile, tile_type_t::wall);
                this->reg.emplace<tile_characteristics_t>(tile);
                this->reg.emplace<vchar_t>(tile, vchar_t{'#', WHITE, BLACK});
            }
        }
    }
};

Map make_test_map(const rect_t& dimension, const position_t& player_pos);

Map new_map(const rect_t& rect);

}  // namespace radl::world
