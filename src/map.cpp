#include <iostream>
#include <algorithm>

#include "spdlog/spdlog.h"

#include "rltk/rltk.hpp"
#include "map.hpp"
#include "utils/utils.hpp"
#include "utils/rng.hpp"

namespace radl::world {

namespace {

using namespace rltk::colors;
// auto walkable_tile = tile_t{
//     true, true, false, floor, /* vchar_t{'#', WHITE, BLACK}, */
// };
// tile_t walkable_tile;

// TODO all calls to corridor to use fill
void fill(entt::registry& reg, Map& map, const rect_t& rect,
          tile_characteristics_t interaction, tile_t tiletype, vchar_t vch) {
    auto xi = std::min(rect.x1, rect.x2);
    auto xf = xi + rect.width();
    auto yi = std::min(rect.y1, rect.y2);
    auto yf = yi + rect.height();
    for(int x = xi; x < xf; ++x) {
        for(int y = yi; y < yf; ++y) {
            auto ent = map.at(x, y);
            reg.emplace_or_replace<tile_characteristics_t>(ent, interaction);
            reg.emplace_or_replace<tile_t>(ent, tiletype);
            reg.emplace_or_replace<vchar_t>(ent, vch);
        }
    }
}

void try_apply_room_to_map(entt::registry& reg, Map& map, const rect_t& rect) {
    spdlog::debug("Applying room:\nr1: {}\nr2: {}", map.rect.to_string(),
                  rect.to_string());
    // ## check if dimensions are valid
    if(!rect.is_inside(map.rect)) {
        spdlog::debug("apply_room: Rect not completely inside.");
        return;
    }
    // quadratic algorithm, kinda dumb
    // ## check if overlap with existing rooms
    for(const auto& other_room : map.rooms) {
        if(rect.intersect(other_room)) {
            spdlog::debug("room intersects\n");
            return;
        }
    }
    // ## if everything is ok, create room
    fill(reg, map, rect,
         tile_characteristics_t{
             true,
             true,
         },
         tile_t{
             tile_type_t::floor,
         },
         vchar_t{
             glyph::BLOCK1,
             WHITE,
             BLACK,
         });
    map.rooms.push_back(rect);
}

constexpr int max_rooms = 30 * 4;
constexpr int min_size  = 6;
constexpr int max_size  = 12;


void create_random_rooms(entt::registry& reg, Map& map) {
    for(int i = 0; i < max_rooms; ++i) {
        auto& rect = map.rect;
        int w      = rng::rng.range(min_size, max_size);
        int h      = rng::rng.range(min_size, max_size);
        int x      = rng::rng.range(1, rect.x2 - w - 2);
        int y      = rng::rng.range(1, rect.y2 - h - 2);
        rect_t rect_new{x, y, x + w, y + h};
        try_apply_room_to_map(reg, map, rect_new);
    }
}

void corridor_horizontal(entt::registry& reg, Map& map, int x1, int x2, int y) {
    auto xi                           = std::min(x1, x2);
    auto xf                           = std::max(x1, x2);
    tile_characteristics_t tile_chars = {true, true};
    auto vch                          = vchar_t{
        glyph::BLOCK1,
        WHITE,
        BLACK,
    };
    auto tile = tile_t{
        tile_type_t::floor,
    };
    fill(reg, map, rect_t{xi, y, xf + 1, y + 1}, tile_chars, tile, vch);
}

void corridor_vertical(entt::registry& reg, Map& map, int y1, int y2, int x) {
    auto yi                           = std::min(y1, y2);
    auto yf                           = std::max(y1, y2);
    tile_characteristics_t tile_chars = {true, true};
    auto vch                          = vchar_t{
        glyph::BLOCK1,
        WHITE,
        BLACK,
    };
    auto tile = tile_t{
        tile_type_t::floor,
    };
    fill(reg, map, rect_t{x, yi, x + 1, yf + 1}, tile_chars, tile, vch);
}

void make_corridors_between_rooms(entt::registry& reg, Map& map) {
    // make corridor with adjacent pairs in rooms vector
    adjacent_pairs(map.rooms.begin(), map.rooms.end(),
                   [&](rect_t& r1, rect_t& r2) {
                       const auto& [px, py] = r1.center();
                       const auto& [nx, ny] = r2.center();
                       auto chance          = rng::rng.range(0, 1);
                       if(chance == 0) {
                           corridor_horizontal(reg, map, px, nx, py);
                           corridor_vertical(reg, map, py, ny, nx);
                       }
                       else {
                           corridor_horizontal(reg, map, px, nx, ny);
                           corridor_vertical(reg, map, py, ny, px);
                       }
                   });
}

}  // namespace

Map new_map(entt::registry& reg, const rect_t& rect) {
    Map map;
    map.init(reg, rect);
    create_random_rooms(reg, map);
    make_corridors_between_rooms(reg, map);
    return map;
}

}  // namespace radl::world
