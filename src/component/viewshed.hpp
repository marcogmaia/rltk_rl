#pragma once
#include <vector>
#include "component/position.hpp"
#include "map.hpp"
#include "entt/entt.hpp"
namespace radl::world {

// TODO FoV
struct viewshed_t {
    std::vector<position_t>
        visible_coordinates;  // visible positions in the map
    int range = 16;           // range of visibility
};

// void update_viewshed(entt::registry& r, entt::entity e) ;
void update_viewshed(position_t& player_pos, viewshed_t& vshed, Map& map);

}  // namespace radl::world
