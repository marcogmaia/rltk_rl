#include "component/component.hpp"
#include "move_attack.hpp"
#include "core/engine.hpp"

namespace radl {

using namespace world;

namespace {
void walk(entt::registry& reg, entt::entity ent, const position_t& actual_pos,
          const position_t& target_pos) {
    // auto& map  = reg.ctx<Map>();
    // auto& tile = map.get_tile(reg, actual_pos);
    // tile_t tile = map[actual_pos];
    reg.replace<position_t>(ent, target_pos);
}
}  // namespace

bool move_attack(entt::registry& reg, entt::entity& ent,
                 const position_t& delta_pos) {
    // mark as dirty to trigger an screen update
    auto& actual_pos = reg.get<position_t>(ent);
    auto target_pos  = actual_pos + delta_pos;

    // maybe check if occupies vicinity, or add vicinity component
    auto& map = reg.ctx<Map>();
    const auto& target_tile_chars = map[{target_pos}].characteristics;

    // ## 1. attack if enemy is in the targeted pos
    if(radl::world::is_occupied(reg, target_pos)) {
        // if(is_occupied(reg, target_pos)) {
        // attack
        return false;
    }
    // ## 2. walk if tile is no occupied and walkable
    else if(target_tile_chars.walkable) {
        walk(reg, ent, actual_pos, target_pos);
    }

    // ## 3. do nothing if is wall
    reg.replace<player_t>(ent);
    return true;
}

}  // namespace radl
