#pragma once

#include <cinttypes>
#include <vector>
#include <deque>

#include "core/map.hpp"

namespace radl {

// A dijkstra map is kinda similar to a floodfill
// base_map_t must return a vector of available walkable tiles, and their costs
struct DijkstraMap {
    size_t width;
    size_t height;
    uint32_t max_depth;

    std::vector<double> cost_map;

    void create_empty(size_t width, size_t height) {
        this->width  = width;
        this->height = height;
        cost_map.resize(width * height);
    }

    void clear() {
        cost_map.clear();
    }

    size_t to_index(position_t pos) {
        auto [x, y] = pos;
        return x + y * width;
    }

    /**
     * @brief Builds the Dijkstra map: iterate from each starting point, to each
     * exit provided by BaseMap's exits implementation. Each step adds cost to
     * the current depth, and is discarded if the new depth is further than the
     * current depth.
     * WARNING: Will give incorrect results when used with
     * non-uniform exit costs. Much slower algorithm required to support that.
     *
     * @param starts
     * @param base_map
     *
     */
    void build(std::vector<position_t> starts, Map& base_map) {
        // node 2d position (x, y) and the cost to travel to that node
        std::deque<std::pair<position_t, double>> open_list;
        open_list.resize(cost_map.size());

        for(const auto& start : starts) {
            open_list.push_back(std::make_pair(start, 0.0));
        }

        while(!open_list.empty()) {
            auto [pos, depth] = open_list.front();
            open_list.pop_front();

            auto available_exits = base_map.get_available_exits(pos);

            for(auto [new_pos, add_depth] : available_exits) {
                auto new_idx    = to_index(new_pos);
                auto new_depth  = depth + add_depth;
                auto prev_depth = cost_map[new_idx];
                if(new_depth >= prev_depth) {
                    continue;
                }
                if(new_depth >= max_depth) {
                    continue;
                }
                cost_map[new_idx] = new_depth;
                open_list.push_back(std::make_pair(new_pos, new_depth));
            }
        }
    }
};

}  // namespace radl
