#include "component/component.hpp"
#include "move_attack.hpp"
#include "engine.hpp"

namespace radl {

using namespace world;



bool move_attack(entt::registry& reg, entt::entity& ent,
                 const position_t& delta_pos) {
    // mark as dirty to trigger an screen update
    auto& actual_pos = reg.get<position_t>(ent);
    auto target_pos  = actual_pos + delta_pos;

    // maybe check if occupies vicinity, or add vicinity component


    auto map_ent           = reg.view<Map>()[0];
    auto& map              = reg.get<Map>(map_ent);
    auto target_tile       = map[target_pos];
    auto target_tile_chars = reg.get<tile_characteristics_t>(target_tile);

    // ## 1. attack if enemy is in the targeted pos
    if(radl::world::is_occupied(reg, target_pos)) {
    // if(is_occupied(reg, target_pos)) {
        // attack
        return false;
    }
    // ## 2. walk if tile is no occupied and walkable
    else if(target_tile_chars.walkable) {
        // walk
        auto& tile = map.get_tile(reg, actual_pos);
        tile.entities.remove(ent);
        // tile.entities.push_back(ent);
        reg.emplace_or_replace<position_t>(ent, target_pos);
        map.get_tile(reg, actual_pos).entities.push_back(ent);
    }

    // ## 3. do nothing if is wall
    // XXX tenho que arrumar uma forma de fazer a query com a tile direto, ou só
    reg.emplace_or_replace<player_t>(ent);
    return true;
}

}  // namespace radl
