#pragma once
#include <algorithm>
#include <concepts>
#include <execution>
#include <unordered_set>
#include <vector>

#include "component/position.hpp"
#include "entt/entt.hpp"

namespace radl::component {

struct viewshed_t {
    static constexpr int default_range = 8;

    int range = default_range;  // range of visibility
    std::unordered_set<position_t, PosHasher> visible_coordinates;
    bool dirty
        = true;  // dirty flag signals that the viewshed needs to be updated
};


}  // namespace radl::component
