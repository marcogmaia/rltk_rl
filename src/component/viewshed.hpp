#pragma once
#include <concepts>
#include <algorithm>
#include <execution>
#include <unordered_set>
#include <vector>
#include "component/position.hpp"
#include "core/map.hpp"
#include "entt/entt.hpp"

// #include "boost/container/flat_set.hpp"
#include <unordered_set>

namespace radl::component {

struct viewshed_t {
    static constexpr int default_range = 8;

    int range = default_range;  // range of visibility
    std::unordered_set<position_t, PosHasher> visible_coordinates;
    // boost::container::flat_set<position_t> visible_coordinates;
    bool dirty
        = true;  // dirty flag signals that the viewshed needs to be updated
};

/**
 * @brief
 *
 * @param reg
 * @param ent
 */
void fov_update();

}  // namespace radl::component
