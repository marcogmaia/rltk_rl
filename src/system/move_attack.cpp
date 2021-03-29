#include "component/position.hpp"
#include "component/attacker.hpp"
#include "move_attack.hpp"
#include "engine.hpp"

namespace radl {

using world::Map;

bool move_attack(entt::registry& reg, entt::entity& ent,
                 const position_t& dpos) {
    // mark as dirty to trigger an screen update
    auto& actual_pos     = reg.get<position_t>(ent);
    auto target_pos      = actual_pos + dpos;

    auto view_pos = reg.view<position_t>();

    auto is_occupied = [&view_pos](position_t target_pos) -> bool {
        for(auto e : view_pos) {
            const auto& pos = view_pos.get(e);
            if(target_pos == pos) {
                return true;
            }
        }
        return false;
    };

    auto map_ent    = reg.view<Map>()[0];
    const auto& map = reg.get<Map>(map_ent);

    // ## 1. attack if enemy in the targeted pos
    if(is_occupied(target_pos)) {
        // attack
        return false;
    }
    // ## 2. walk if tile is no occupied and walkable
    else if(map[target_pos].is_walkable) {
        // walk
        // actual_pos = target_pos;
        reg.emplace_or_replace<position_t>(ent, target_pos);
    }

    // ## 3. do nothing if is wall
    // XXX tenho que arrumar uma forma de farzer query com a tile direto, ou só
    // checo o mapa
    return true;
}

}  // namespace radl
