#include <algorithm>
#include <execution>

#include "entt/entt.hpp"
#include "spdlog/spdlog.h"

#include "core/engine.hpp"
#include "component/viewshed.hpp"
#include "system/visibility.hpp"

namespace radl::world {
// void for_every_ent_in_tiles_in_view(entt::entity ent) {
//     auto viewshed = engine::reg.get<viewshed_t>(ent);
//     auto w_map = Map::get_map(engine::reg);
//     for(auto &visible_pos: viewshed.visible_coordinates) {
//         auto &tile = Map::get_tile(engine::reg, w_map, visible_pos);
//         for(auto &ent_in_tile: tile.entities) {
//             // isso tá muito errado!
//         }
//     }
// }
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
void fov_update(entt::registry& reg, entt::entity ent) {
    using namespace world;
    auto& map     = reg.ctx<Map>();
    auto& vshed   = reg.get<viewshed_t>(ent);
    auto& ent_pos = reg.get<position_t>(ent);

    // isso gera alocações temporarias pkct
    vshed.visible_coordinates.clear();

    auto set_visibility = [&](position_t reveal_pos) {
        if(ent == engine::player && map.rect.contains(reveal_pos)) {
            auto& reveal_tile = map.at(reveal_pos);
            vshed.visible_coordinates.emplace_back(reveal_pos);
            reveal_tile.characteristics.explored = true;
        }
    };

    auto is_transparent = [&](position_t check_pos) {
        if(map.rect.contains(check_pos)) {
            auto &tile = map[check_pos];
            return tile.characteristics.transparent;
        }
        return false;
    };

    radl::visibility_sweep_2d<position_t, nav_helper>(
        ent_pos, vshed.range, set_visibility, is_transparent);
}

//   for(auto& e_ent : *get_entities_near_player()) {
//             fov_update(reg, e_ent);
//         }

}  // namespace radl::world
