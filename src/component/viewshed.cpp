#include "component/viewshed.hpp"
#include "entt/entt.hpp"
#include "spdlog/spdlog.h"
#include "core/engine.hpp"

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
}  // namespace radl::world
