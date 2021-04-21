#pragma once

#include <cinttypes>
#include <tuple>
#include <vector>

#include "core/map/map.hpp"

// objectives, need to think more about this problem (multiple objectives)

// TODO make the DM a template to be useful for every case

namespace radl {

// A dijkstra map is as flow field
// base_map_t must return a vector of available walkable tiles, and their costs
struct DijkstraMap {
    size_t width;
    size_t height;
    uint32_t max_depth = 32;
    bool dirty         = true;

    std::vector<double> cost_map;

    const Map& base_map;

    DijkstraMap(const Map& base_map);

    ~DijkstraMap() = default;

    inline void clear() {
        std::ranges::fill(cost_map, max_depth);
    }

    inline void create_empty(size_t width, size_t height) {
        this->width  = width;
        this->height = height;
        cost_map.resize(width * height);
        clear();
    }

    inline void create_empty(const Map& map) {
        create_empty(map.rect.width(), map.rect.height());
    }

    inline size_t to_index(position_t pos) {
        auto [x, y] = pos;
        return x + y * width;
    }

    inline position_t from_index(size_t idx) {
        auto x = idx % width;
        auto y = idx / width;
        return position_t{x, y};
    }

    inline double at(position_t pos) {
        auto [x, y] = pos;
        return cost_map[x + y * width];
    }

    /**
     * @brief Builds the Dijkstra map: iterate from each starting point, to each
     * exit provided by BaseMap's exits implementation. Each step adds cost to
     * the current depth, and is discarded if the new depth is further than the
     * current depth.
     * @warning Will give incorrect results when used with
     * non-uniform exit costs. Much slower algorithm required to support that.
     * @note: only recomputes when the dirty flag is set, the function will
     * set the flag to false when compotation begins
     *
     * @param starts
     * @param base_map
     *
     */
    void compute(std::vector<position_t> starts);

    /**
     * @brief finds one of the possible lowest paths
     *
     * @param pos starting position
     * @return position_t of a lower path
     */
    std::tuple<bool, position_t> pick_random_min_path(const position_t& pos);
};

}  // namespace radl
