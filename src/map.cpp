#include <iostream>
#include <algorithm>
#include <fmt/format.h>

#include <utility>
#include "spdlog/spdlog.h"

#include "libtcod.hpp"
#include "map.hpp"

namespace radl {

namespace {

auto walkable_tile = tile_t{false, true, false, floor};

bool is_inside(const rect_t& out, const rect_t& in) {
    return out.x1 < in.x1 && out.x2 > in.x2 && out.y1 < in.y1 && out.y2 > in.y2;
}

void fill(Map& map, const rect_t& rect, const tile_t& tile) {
    // ## check if dimensions are valid
    spdlog::debug("calling fill:\nr1: {}\nr2: {}\n", map.rect.to_string(),
                 rect.to_string());

    if(!is_inside(map.rect, rect)) {
        spdlog::info("map.fill: Rect not completely inside.\n");
        return;
    }


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

void apply_room_to_map(Map& map, const rect_t& rect) {
    fill(map, rect, walkable_tile);
}

}  // namespace

Map make_test_map(const rect_t& rect, const position_t& player_pos) {
    Map map = {
        .rect  = rect,
        .tiles = std::vector<tile_t>(rect.area()),
    };
    auto& tiles = map.tiles;

    fill(map, {rect.x1 + 1, rect.y1 + 1, rect.x2 - 1, rect.y2 - 1},
         walkable_tile);

    // fill randomly with walls 33% chance
    auto& rng = *TCODRandom::getInstance();
    for(auto& tile : tiles) {
        if(tile.is_walkable) {
            auto chance = rng.getInt(1, 3);
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
    };

    rect_t r1 = {4, 5, 10, 25};
    rect_t r2 = {11, 34, 6, 8};
    auto cpos = map.rect.center();
    rect_t r3 = {cpos.x - 3, cpos.y - 3, cpos.x + 3, cpos.y + 3};
    apply_room_to_map(map, r1);
    apply_room_to_map(map, r2);
    apply_room_to_map(map, r3);

    return map;
}

}  // namespace radl