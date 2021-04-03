#include "component/component.hpp"
#include "core/map.hpp"
#include "core/engine.hpp"

namespace radl {

// using engine::reg;
void ai_sys(entt::registry& reg, entt::entity ent) {
    // make group of all enemies in active
    // auto enemy_group = reg.group
    using engine::player;
    using namespace world;
    // XXX the enemies aren't marked as visible, the tiles are.
    // reg.group<position_t>(entt::get<enemy_t>)
    //     .each([](auto e_enemy, position_t& pos) {
    //         auto [x, y] = pos;
    //         spdlog::info("enemy at ({}, {}) sees you.", x, y);
    //     });
}

}  // namespace radl
