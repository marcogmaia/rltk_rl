#include <algorithm>
#include <ranges>
#include "core/map/dijkstra_map.hpp"

namespace radl {

DijkstraMap::DijkstraMap(const Map& base_map)
    : base_map(base_map) {
    create_empty(base_map);
}

void DijkstraMap::compute(std::vector<position_t> starts) {
    if(!dirty) {
        return;
    }
    dirty = false;
    clear();
    // node 2d position (x, y) and the cost to travel to that node
    std::deque<std::pair<position_t, double>> open_list;

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

// FIXME not fault tolerant, should return a value to indicate that the position
// ! is valid
std::pair<bool, position_t>
DijkstraMap::find_lowest_path(const position_t& pos) {
    auto available_paths    = base_map.get_available_exits(pos);
    using pair_pos_weight_t = decltype(available_paths[0]);
    std::vector<std::pair<position_t, int>> vec_pos_weight;
    vec_pos_weight.reserve(8);
    bool valid_pos = true;
    for(auto [avail_pos, ignore_] : available_paths) {
        auto cost = at(avail_pos);
        vec_pos_weight.emplace_back(std::make_pair(avail_pos, cost));
    }
    std::ranges::sort(
        vec_pos_weight,
        [&available_paths](pair_pos_weight_t& lhs, pair_pos_weight_t& rhs) {
            return lhs.second < rhs.second;
        });

    auto [target_pos, weight] = vec_pos_weight[0];
    if(weight == max_depth) {
        valid_pos = false;
    }
    return std::make_pair(valid_pos, target_pos);
}

}  // namespace radl