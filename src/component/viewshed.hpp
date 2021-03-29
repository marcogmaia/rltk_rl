#pragma once
#include <vector>
#include "component/position.hpp"
// #include "map.hpp"

namespace radl::world {

// TODO FoV
struct viewshed_t {
    std::vector<position_t> visible_pos;  // visible positions in the map
    int range = 8;                        // range of visibility
};

}  // namespace radl::world