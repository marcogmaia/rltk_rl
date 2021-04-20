#include <algorithm>
#include <deque>
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

std::tuple<bool, position_t>
DijkstraMap::find_lowest_path(const position_t& pos) {
    namespace ranges     = std::ranges;
    auto available_paths = base_map.get_available_exits(pos);
    if(available_paths.empty()) {
        return {false, {0, 0}};
    }
    using pair_pos_weight_t = decltype(available_paths)::value_type;
    std::vector<std::pair<position_t, int>> vec_pos_weight;
    vec_pos_weight.reserve(8);
    bool valid_pos = true;
    for(auto [avail_pos, ignore_] : available_paths) {
        auto cost = at(avail_pos);
        vec_pos_weight.emplace_back(std::make_pair(avail_pos, cost));
    }
    ranges::sort(vec_pos_weight,
                 [](pair_pos_weight_t& lhs, pair_pos_weight_t& rhs) {
                     return lhs.second < rhs.second;
                 });

    auto ret                   = vec_pos_weight[0];
    auto& [target_pos, weight] = ret;  // binds as a reference
    if(static_cast<uint32_t>(weight) == max_depth) {
        valid_pos = false;
    }

    // vec_pos_weight[0] is the minimum element, so we search for the partition
    // point and pick a random value there (in the partition)
    auto partition_pt = ranges::partition_point(
        vec_pos_weight, [&vec_pos_weight](pair_pos_weight_t& val) {
            return val.second == vec_pos_weight[0].second;
        });
    rng::rng.random_choice(vec_pos_weight.begin(), partition_pt, &ret);

    return {valid_pos, target_pos};
}

}  // namespace radl
