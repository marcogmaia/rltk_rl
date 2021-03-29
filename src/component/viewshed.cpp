#include "component/viewshed.hpp"
#include "entt/entt.hpp"
#include "spdlog/spdlog.h"

// TODO remove this function from here, it is not a componenet neither a system,
// I think it belongs to the camera system

namespace radl::world {

// void update_viewshed(entt::registry& r, entt::entity ent) {
//     auto& [px, py]               = r.get<position_t>(ent);
//     auto& [visible_tiles, range] = r.get<viewshed_t>(ent);
// auto& map = r.get<Map>(r.view<Map>().front());
void update_viewshed(position_t& player_pos, viewshed_t& vshed,
                     Map& map) {
    auto& [px, py] = player_pos;
    auto& range    = vshed.range;

    auto xi = px - range;
    auto xf = px + range;
    auto yi = py - range;
    auto yf = py + range;

    vshed.visible_coordinates.clear();
    for(int x = xi; x < xf; ++x) {
        for(int y = yi; y < yf; ++y) {
            auto pos = position_t{x, y};
            if(map.rect.contains(pos)) {  // && !map[pos].is_visible) {
                map[pos].is_explored = true;
                vshed.visible_coordinates.push_back(pos);
            }
        }
    }
}

}  // namespace radl::world
