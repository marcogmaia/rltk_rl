#pragma once
#include <vector>
#include "component/position.hpp"
#include "core/map.hpp"
#include "entt/entt.hpp"

#include <set>
namespace radl::world {

namespace {
    constexpr int default_range = 8 ;
}
struct viewshed_t {
    int range = default_range;  // range of visibility
    std::vector<position_t> visible_coordinates;
};

// void update_viewshed(entt::registry& r, entt::entity e) ;
void update_viewshed(position_t& player_pos, viewshed_t& viewshed, Map& map);

}  // namespace radl::world
