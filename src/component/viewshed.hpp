#pragma once
#include <algorithm>
#include <execution>
#include <unordered_set>
#include <vector>
#include "component/position.hpp"
#include "core/map.hpp"
#include "entt/entt.hpp"

namespace std {
template <>
struct hash<radl::position_t> {
    std::size_t operator()(const radl::position_t& pos) const {
        return (hash<int>()(pos.first) ^ (hash<int>()(pos.second) << 1)) >> 1;
    }
};
}  // namespace std

namespace radl::world {

namespace {
constexpr int default_range = 8;
}


struct viewshed_t {
    int range = default_range;  // range of visibility
    std::vector<position_t> visible_coordinates;
};

// void update_viewshed(entt::registry& r, entt::entity e) ;
void update_viewshed(position_t& player_pos, viewshed_t& viewshed, Map& map);

void fov_update(entt::registry& reg, entt::entity ent);
// void fov_update_parallel() ;


template <typename Entt_group>
void fov_update_parallel(Entt_group group) {
    using engine::reg;
    // auto group = reg.group<viewshed_t>();
    std::for_each(std::execution::par_unseq, group.begin(), group.end(),
                  [](auto& ent) {
                      fov_update(engine::reg, ent);
                  });
}


}  // namespace radl::world
