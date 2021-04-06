#include <algorithm>
#include <execution>

#include "entt/entt.hpp"
#include "spdlog/spdlog.h"

#include "core/engine.hpp"
#include "component/viewshed.hpp"
#include "system/visibility.hpp"

namespace radl::world {


namespace {


struct nav_helper {
    static int get_x(const position_t& pos) {
        return pos.first;
    }
    static int get_y(const position_t& pos) {
        return pos.second;
    }
    static position_t get_xy(const int& x, const int& y) {
        return position_t{x, y};
    }
};

}  // namespace

// make this update parallel
void fov_update(entt::registry& reg, const entt::entity& ent) {
    using namespace world;
    auto& map   = engine::get_map();
    auto& vshed = reg.get<viewshed_t>(ent);
    if(!vshed.dirty) {
        return;
    }
    vshed.dirty   = false;
    auto& ent_pos = reg.get<position_t>(ent);

    decltype(vshed.visible_coordinates) new_vis;
    // new_vis.reserve(vshed.range * vshed.range);

    auto set_visibility = [&](const position_t& reveal_pos) {
        if(map.rect.contains(reveal_pos)) {
            auto& reveal_tile = map.at(reveal_pos);
            new_vis.insert(reveal_pos);
            if(ent == engine::player) {
                reveal_tile.characteristics.explored = true;
            }
        }
    };

    auto is_transparent = [&](const position_t& check_pos) {
        if(map.rect.contains(check_pos)) {
            auto& tile = map[check_pos];
            return tile.characteristics.transparent;
        }
        return false;
    };

    radl::visibility_sweep_2d<position_t, nav_helper>(
        ent_pos, vshed.range, set_visibility, is_transparent);
    vshed.visible_coordinates.swap(new_vis);
}

}  // namespace radl::world
