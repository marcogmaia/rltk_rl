#include <algorithm>
#include <ranges>
#include <execution>
#include <memory>
#include "component/component.hpp"
#include "core/map.hpp"
#include "core/engine.hpp"
#include "system/player_action.hpp"


#include "utils/path_finding.hpp"

namespace radl {

void ai_enemy_find_path(entity e_ent, const position_t& target_pos) {
    using namespace world;
    using engine::reg;
    viewshed_t& e_vshed = reg.get<viewshed_t>(e_ent);
    auto& vis_coords    = e_vshed.visible_coordinates;

    if(!vis_coords.contains(target_pos)) {
        return;
    }
    // found
    auto e_pos = reg.get<position_t>(e_ent);
    if(e_pos != target_pos) {
        auto path = find_path_2d<position_t, navigator<position_t>>(e_pos,
                                                                    target_pos);
        if(path->success && path->steps.size() > 0) {
            auto next_step = path->steps.front();
            path->steps.pop_front();
            auto& map = reg.ctx<Map>();
            if(map.rect.contains(next_step)) {
                move_attack(engine::reg, e_ent, next_step);
            }
        }
    }
}

// using engine::reg;
void ai_enemy(entt::registry& reg) {
    using engine::player;
    using namespace world;

    auto g_enemies  = reg.group<being_t, position_t>();
    auto player_pos = reg.get<position_t>(player);

    auto func = [&](entity ent) {
        if(player == ent) {
            return;
        }
        auto& e_vshed    = reg.get<viewshed_t>(ent);
        auto& vis_coords = e_vshed.visible_coordinates;
        if(vis_coords.contains(player_pos)) {
            if(ent != player) {
                ai_enemy_find_path(ent, player_pos);
            }
        }
    };

    std::for_each(std::execution::par_unseq, g_enemies.begin(), g_enemies.end(),
                  func);
}

}  // namespace radl
