#pragma once
#include <vector>
#include "component/position.hpp"
#include "map.hpp"
#include "entt/entt.hpp"

#include <set>
namespace radl::world {

struct viewshed_t {
    int range = 8;  // range of visibility
    std::vector<position_t> visible_coordinates;
};

// void update_viewshed(entt::registry& r, entt::entity e) ;
void update_viewshed(position_t& player_pos, viewshed_t& vshed, Map& map);

}  // namespace radl::world
