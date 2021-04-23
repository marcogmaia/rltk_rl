#include <algorithm>
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

namespace radl::system {

namespace {
[[maybe_unused]] void timeit(const std::function<void(void)>& func) {
    sf::Clock clk;
    clk.restart();
    func();
    spdlog::info("\tran in: {}us", clk.getElapsedTime().asMicroseconds());
}
}  // namespace

/**
 * @brief Update entitis viewshet parallel
 *
 */
void fov_update() {
    // FIXME this must be a system
    sf::Clock clk;
    clk.restart();

    const auto& view_vshed = reg.view<viewshed_t>();

    // auto& ents_near = *get_entities_near_player();
    std::vector<entity> ents_to_calculate_fov;
    // ents_to_calculate_fov.reserve(64);
    // std::ranges::copy_if(ents_near,
    // std::back_inserter(ents_to_calculate_fov),
    //                      [&view_vshed](auto ent) {
    //                          return view_vshed.contains(ent);
    //                      });

    int count_fovs_updated = 0;
    static std::mutex cntmutex;

    auto run_fov_sweep = [&](auto ent) {
        auto& vshed = view_vshed.get<viewshed_t>(ent);
        if(!vshed.dirty) {
            return;
        }
        vshed.dirty   = false;
        auto& map     = get_map();
        auto& ent_pos = reg.get<position_t>(ent);

        decltype(vshed.visible_coordinates) new_vis;

        auto set_visibility = [&](const position_t& reveal_pos) {
            if(map.rect.contains(reveal_pos)) {
                auto& reveal_tile = map.at(reveal_pos);
                new_vis.insert(reveal_pos);
                if(ent == player) {
                    reveal_tile.props.explored = true;
                }
            }
        };

        auto is_transparent = [&](const position_t& check_pos) {
            if(map.rect.contains(check_pos)) {
                auto& tile = map[check_pos];
                return tile.props.transparent;
            }
            return false;
        };

        visibility_sweep_2d<position_t, navigator_t<position_t>>(
            ent_pos, vshed.range, set_visibility, is_transparent);
        vshed.visible_coordinates.swap(new_vis);
        std::lock_guard lock(cntmutex);
        ++count_fovs_updated;
    };

    auto v_ents_to_update = reg.view<viewshed_t>();
    std::for_each(std::execution::par_unseq, v_ents_to_update.begin(),
                  v_ents_to_update.end(), run_fov_sweep);

    // spdlog::info("updating_fov {} entities", ents_to_calculate_fov.size());

    // std::for_each(std::execution::par_unseq, ents_to_calculate_fov.begin(),
    //               ents_to_calculate_fov.end(), run_fov_sweep);
    // run_fov_sweep(player);

    // spdlog::info("\tupdated: {} fovs, dT: {}", count_fovs_updated,
    //              clk.getElapsedTime().asMicroseconds());
}

}  // namespace radl::system
