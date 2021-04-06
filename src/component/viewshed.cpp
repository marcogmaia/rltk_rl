#include <algorithm>
#include <execution>

#include "entt/entt.hpp"
#include "spdlog/spdlog.h"

#include "core/engine.hpp"
#include "component/viewshed.hpp"
#include "utils/path_finding.hpp"
#include "system/visibility.hpp"

namespace radl::world {

/**
 * @brief Update entitis viewshet parallel
 *
 */
void fov_update() {
    using namespace world;
    using engine::reg;

    const auto& view_vshed = reg.view<viewshed_t>();

    auto run_fov_sweep = [&view_vshed](auto ent) {
        auto& vshed = view_vshed.get<viewshed_t>(ent);
        if(!vshed.dirty) {
            return;
        }
        vshed.dirty   = false;
        auto& map     = engine::get_map();
        auto& ent_pos = reg.get<position_t>(ent);

        decltype(vshed.visible_coordinates) new_vis;

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

        radl::visibility_sweep_2d<position_t, navigator_t<position_t>>(
            ent_pos, vshed.range, set_visibility, is_transparent);
        vshed.visible_coordinates.swap(new_vis);
    };

    spdlog::info("updating_fov {} entities", view_vshed.size());
    std::for_each(view_vshed.begin(), view_vshed.end(), run_fov_sweep);
}

}  // namespace radl::world
