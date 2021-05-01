#include <algorithm>
#include <tuple>
#include <execution>
#include <ranges>

#include "SFML/Window.hpp"
#include "spdlog/spdlog.h"

#include "component/viewshed.hpp"
#include "component/position.hpp"

#include "system/visibility.hpp"
#include "system/game_state.hpp"
#include "system/map/map.hpp"

#include "utils/path_finding.hpp"
#include "utils/fov.h"

namespace radl::system {

namespace {

[[maybe_unused]] void timeit(const std::function<void(void)>& func) {
    sf::Clock clk;
    clk.restart();
    func();
    spdlog::info("\tran in: {}us", clk.getElapsedTime().asMicroseconds());
}



using fov_user_type_t = std::tuple<entity, radl::Map*,
                                   std::unordered_set<position_t, PosHasher>*>;

bool is_opaque(void* pmap, int x, int y) {
    // auto& map = *static_cast<radl::Map*>(pmap);
    auto& [ent, map, vshed] = *static_cast<fov_user_type_t*>(pmap);
    position_t check_pos{x, y};
    position_t ent_pos = reg.get<position_t>(ent);
    if(check_pos == ent_pos) {
        return false;
    }
    if(map->rect.contains(check_pos)) {
        auto& tile = map->at(check_pos);
        return !tile.props.transparent;
    }
    return true;
}

// pmap should be a pair of ptrs to map and viewshed
void set_visibility(void* pmap, int x, int y, int dx, int dy, void* src) {
    auto& [ent, map, vis] = *static_cast<fov_user_type_t*>(pmap);
    position_t vis_pos{x, y};
    if(map->rect.contains(vis_pos)) {
        if(ent == player) {
            map->at(vis_pos).props.explored = true;
        }
        vis->insert(vis_pos);
        // vis->visible_coordinates.insert(vis_pos);
    }
}

fov_settings_type settings{
    .opaque       = is_opaque,
    .apply        = set_visibility,
    .shape        = FOV_SHAPE_CIRCLE_PRECALCULATE,
    .corner_peek  = FOV_CORNER_NOPEEK,
    .opaque_apply = FOV_OPAQUE_APPLY,
    .heights      = NULL,
    .numheights   = 0,
};

}  // namespace

/**
 * @brief Update entitis viewshed parallel
 *
 */
void fov_update() {
    // FIXME this must be a system
    const auto& view_vshed = reg.view<viewshed_t>();
    auto& map = get_map();

    // fov_settings_set_opacity_test_function(&settings, is_opaque);
    // fov_settings_set_apply_lighting_function(&settings, set_visibility);

    std::for_each(std::execution::par_unseq, view_vshed.begin(),
                  view_vshed.end(), [&map, &view_vshed](entity ent) {
                      auto& vshed        = view_vshed.get<viewshed_t>(ent);
                      position_t src_pos = reg.get<position_t>(ent);
                      decltype(vshed.visible_coordinates) new_vis;
                      new_vis.insert(src_pos);
                      fov_user_type_t s_map{
                          ent,
                          &map,
                          &new_vis,
                      };
                      auto [x, y] = src_pos;
                      fov_circle(&settings, &s_map, nullptr, x, y, vshed.range);
                      vshed.visible_coordinates.swap(new_vis);
                  });
}

}  // namespace radl::system
