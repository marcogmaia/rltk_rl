#include <iostream>
#include <algorithm>
#include <fmt/format.h>

#include <utility>


#include "libtcod.hpp"
#include "map.hpp"

namespace radl {

Map::Map(const rect_t& dimension)
    : dimensions(dimension) {
    tiles.resize(dimension.w * dimension.h);
    auto tilefill = tile_t{false, true, false, floor};
    fill({dimensions.x + 1, dimensions.y + 1, dimensions.w - 1,
          dimensions.h - 1},
         tilefill);
}

void Map::fill(const rect_t& rect, const tile_t& tile) {
    const auto& xi = dimensions.x;
    const auto& yi = dimensions.y;
    auto xf        = xi + rect.w;
    auto yf        = yi + rect.h;
    for(int x = rect.x; x < xf; ++x) {
        for(int y = rect.y; y < yf; ++y) {
            tiles[x + y * dimensions.w] = tile;
        }
    }
}


}  // namespace radl