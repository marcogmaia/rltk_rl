#include "component/component.hpp"
#include "move_attack.hpp"
#include "engine.hpp"

namespace radl {

using namespace world;


bool move_attack(entt::registry& reg, entt::entity& ent,
                 const position_t& dpos) {
    // mark as dirty to trigger an screen update
    auto& actual_pos = reg.get<position_t>(ent);
    auto target_pos  = actual_pos + dpos;

    // maybe check if occupies vicinity, or add vicinity component
    auto view_pos    = reg.view<destructible_t, renderable_t, position_t>();
    auto is_occupied = [&view_pos, &actual_pos](position_t target_pos) -> bool {
        for(auto e : view_pos) {
            const auto& pos = view_pos.get<position_t>(e);
            if(target_pos == pos) {
                return true;
            }
        }
        return false;
    };

    auto map_ent           = reg.view<Map>()[0];
    auto& map        = reg.get<Map>(map_ent);
    auto target_tile       = map[target_pos];
    auto target_tile_chars = reg.get<tile_characteristics_t>(target_tile);

    // ## 1. attack if enemy in the targeted pos
    if(is_occupied(target_pos)) {
        // attack
        // return false;
    }
    // ## 2. walk if tile is no occupied and walkable
    else if(target_tile_chars.walkable) {
        // walk
        auto &tile = map.get_tile(reg, actual_pos);
        tile.entities.remove(ent);
        // tile.entities.push_back(ent);
        reg.emplace_or_replace<position_t>(ent, target_pos);
        map.get_tile(reg, actual_pos).entities.push_back(ent);
    }

    // ## 3. do nothing if is wall
    // XXX tenho que arrumar uma forma de farzer query com a tile direto, ou só
    // checo o mapa
    return true;
}

}  // namespace radl
