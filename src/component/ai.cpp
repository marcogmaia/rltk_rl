#include <algorithm>
#include <execution>
#include "component/component.hpp"
#include "core/map.hpp"
#include "core/engine.hpp"

namespace radl {

// using engine::reg;
void ai_sys(entt::registry& reg) {
    // make group of all enemies in active
    // auto enemy_group = reg.group
    using engine::player;
    using namespace world;
    // XXX the enemies aren't marked as visible, the tiles are.
    auto g_enemies = reg.group<enemy_t, renderable_t, position_t, viewshed_t>();
    auto player_pos = reg.get<position_t>(player);

    auto func = [&g_enemies, player_pos](auto ent) {
        const auto& vshed = g_enemies.get<viewshed_t>(ent);
        const auto& e_pos = g_enemies.get<position_t>(ent);

        for(auto& v_pos : vshed.visible_coordinates) {
            if(v_pos == player_pos) {
                auto [x, y] = e_pos;
                spdlog::info("enemy at ({}, {}) sees you.", x, y);
                break;
            }
        }
    };

    std::for_each(std::execution::par_unseq, g_enemies.begin(), g_enemies.end(),
                  func);
}

}  // namespace radl
