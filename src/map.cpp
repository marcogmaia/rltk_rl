#include <iostream>
#include <algorithm>

#include "spdlog/spdlog.h"

#include "rltk/rltk.hpp"
#include "map.hpp"
#include "utils/utils.hpp"
#include "utils/rng.hpp"

namespace radl::world {

namespace {

auto walkable_tile = tile_t{false, true, false, floor};

void fill(Map& map, const rect_t& rect, const tile_t& tile) {
    auto xi = std::min(rect.x1, rect.x2);
    auto xf = xi + rect.width();
    auto yi = std::min(rect.y1, rect.y2);
    auto yf = yi + rect.height();
    for(int x = xi; x < xf; ++x) {
        for(int y = yi; y < yf; ++y) {
            map.tiles[x + y * map.rect.width()] = tile;
        }
    }
}

void try_apply_room_to_map(Map& map, const rect_t& rect) {
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
    fill(map, rect, walkable_tile);
    map.rooms.push_back(rect);
}

constexpr int max_rooms = 30;
constexpr int min_size  = 6;
constexpr int max_size  = 12;


void create_random_rooms(Map& map) {
    for(int i = 0; i < max_rooms; ++i) {
        auto& rect = map.rect;
        int w      = rng::rng.range(min_size, max_size);
        int h      = rng::rng.range(min_size, max_size);
        int x      = rng::rng.range(1, rect.x2 - w - 2);
        int y      = rng::rng.range(1, rect.y2 - h - 2);
        rect_t rect_new{x, y, x + w, y + h};
        try_apply_room_to_map(map, rect_new);
    }
}

void corridor_horizontal(Map& map, int x1, int x2, int y) {
    auto xi = std::min(x1, x2);
    auto xf = std::max(x1, x2);
    for(int x = xi; x <= xf; ++x) {
        tile_t tile{
            .is_transparent = true,
            .is_walkable    = true,
            .is_explored    = false,
            .type           = tile_type_t::floor,
        };
        map.at(x, y) = tile;
    }
}

void corridor_vertical(Map& map, int y1, int y2, int x) {
    auto yi = std::min(y1, y2);
    auto yf = std::max(y1, y2);
    for(int y = yi; y <= yf; ++y) {
        tile_t tile{
            .is_transparent = true,
            .is_walkable    = true,
            .is_explored    = false,
            .type           = tile_type_t::floor,
        };
        map.at(x, y) = tile;
    }
}


void make_corridors_between_rooms(radl::world::Map& map) {
    // make corridor with adjacent pairs in rooms vector
    adjacent_pairs(map.rooms.begin(), map.rooms.end(),
                   [&](rect_t& r1, rect_t& r2) {
                       const auto& [px, py] = r1.center();
                       const auto& [nx, ny] = r2.center();
                       auto chance          = rng::rng.range(0, 1);
                       if(chance == 0) {
                           corridor_horizontal(map, px, nx, py);
                           corridor_vertical(map, py, ny, nx);
                       }
                       else {
                           corridor_horizontal(map, px, nx, ny);
                           corridor_vertical(map, py, ny, px);
                       }
                   });
}

}  // namespace

Map make_test_map(const rect_t& rect, const position_t& player_pos) {
    Map map = {
        .rect  = rect,
        .tiles = std::vector<tile_t>(rect.area()),
        .rooms = std::vector<rect_t>(),
    };
    auto& tiles = map.tiles;

    fill(map, {rect.x1 + 1, rect.y1 + 1, rect.x2 - 1, rect.y2 - 1},
         walkable_tile);

    // fill randomly with walls 33% chance
    // auto& rng = *TCODRandom::getInstance();
    for(auto& tile : tiles) {
        if(tile.is_walkable) {
            auto chance = rng::rng.range(1, 3);
            if(chance == 1) {
                tile.is_walkable = false;
                tile.type        = wall;
            }
        }
    }
    auto& player_start_tile       = map.at(player_pos);
    player_start_tile.is_walkable = true;
    player_start_tile.type        = floor;
    return map;
}


Map new_map(const rect_t& rect, const position_t& player_pos) {
    Map map = {
        .rect  = rect,
        .tiles = std::vector<tile_t>(rect.area()),
        .rooms = std::vector<rect_t>(),
    };
    create_random_rooms(map);
    make_corridors_between_rooms(map);
    return map;
}

}  // namespace radl::world
