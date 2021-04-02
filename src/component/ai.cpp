#include "component/component.hpp"
#include "core/map.hpp"

namespace radl {

// using engine::reg;
void ai_sys(entt::registry& reg, entt::entity ent) {
    using engine::player;
    using namespace world;
    reg.group<position_t>(entt::get<enemy_t, visible_t>)
        .each([](auto e_enemy, position_t& pos) {
            ;
        });
}

}  // namespace radl
