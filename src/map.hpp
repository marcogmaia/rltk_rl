#pragma once

#include <memory>
#include <vector>
#include <list>
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
    // list because I don't want realocations
    std::list<entt::entity> entities;
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

    inline tile_t& get_tile(entt::registry& reg, position_t pos) {
        auto tent  = at(pos);
        auto& tile = reg.get<tile_t>(tent);
        return tile;
    }

    inline void init(entt::registry& reg, const rect_t& r) {
        // m_reg     = reg;
        rect      = r;
        auto area = r.area();
        tiles.resize(area);

        for(int x = 0; x < r.width(); ++x) {
            for(int y = 0; y < r.height(); ++y) {
                auto tile                = reg.create();
                tiles[x + y * r.width()] = tile;
                reg.emplace<position_t>(tile, position_t{x, y});
                reg.emplace<tile_t>(tile, tile_type_t::wall);
                reg.emplace<tile_characteristics_t>(tile);
                reg.emplace<vchar_t>(tile, vchar_t{
                                               glyph::SOLID1,
                                               WHITE,
                                               BLACK,
                                           });
            }
        }
    }
};

Map make_test_map(const rect_t& dimension, const position_t& player_pos);

Map new_map(entt::registry& reg, const rect_t& rect);

}  // namespace radl::world
