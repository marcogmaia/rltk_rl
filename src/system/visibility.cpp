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

#include "utils/permissive-fov/permissive-fov.hpp"

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

void set_visibility(void* pmap, int x, int y, int dx, int dy, void* src) {
    auto& [ent, map, vis] = *static_cast<fov_user_type_t*>(pmap);
    position_t vis_pos{x, y};
    if(map->rect.contains(vis_pos)) {
        if(ent == player) {
            map->at(vis_pos).props.explored = true;
        }
        vis->insert(vis_pos);
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

class private_fov_t {
private:
    entity m_ent;
    radl::Map& m_map;
    std::unordered_set<position_t, PosHasher>& m_vis;

public:
    private_fov_t(entity ent, radl::Map& map,
                  std::unordered_set<position_t, PosHasher>& vis)
        : m_ent(ent)
        , m_map(map)
        , m_vis(vis) {}

    bool isBlocked(int destX, int destY) {
        position_t check_pos{destX, destY};
        const position_t& ent_pos = reg.get<position_t>(m_ent);
        if(check_pos == ent_pos) {
            return false;
        }
        if(m_map.rect.contains(check_pos)) {
            auto& tile = m_map.at(check_pos);
            return !tile.props.transparent;
        }
        return true;
    }

    void visit(int destX, int destY) {
        position_t vis_pos{destX, destY};
        if(m_map.rect.contains(vis_pos)) {
            if(m_ent == player) {
                m_map.at(vis_pos).props.explored = true;
            }
            m_vis.insert(vis_pos);
        }
    }
};

}  // namespace

/**
 * @brief Update entities viewshed parallel
 *
 */
void fov_update() {
    const auto& view_vshed = reg.view<viewshed_t>();
    auto& map              = get_map();

    std::for_each(std::execution::par_unseq, view_vshed.begin(),
                  view_vshed.end(), [&map, &view_vshed](entity ent) {
                      auto& vshed = view_vshed.get<viewshed_t>(ent);
                      decltype(vshed.visible_coordinates) new_vis;
                      position_t src_pos = reg.get<position_t>(ent);
                      new_vis.insert(src_pos);
                      private_fov_t pfov{ent, map, new_vis};
                      auto [x, y] = src_pos;
                      permissive::squareFov(x, y, vshed.range, pfov);
                      vshed.visible_coordinates.swap(new_vis);
                  });
}

}  // namespace radl::system
