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


namespace {

using engine::reg;
using namespace world;

[[maybe_unused]] bool surrounded(const position_t& pos) {
    auto& map = engine::get_map();
    for(int dx = -1; dx <= 1; ++dx) {
        for(int dy = -1; dy <= 1; ++dy) {
            if(dx == 0 && dy == 0) {
                continue;
            }
            position_t offset = {dx, dy};
            auto look_pos     = pos + offset;
            auto walkable     = map[look_pos].characteristics.walkable;
            auto can_walk     = !is_occupied(look_pos) && walkable;
            if(can_walk) {
                return false;
            }
        }
    }
    return true;
}

[[maybe_unused]] bool is_near(const position_t& src_pos,
                              const position_t& dst_pos) {
    auto delta_pos = dst_pos - src_pos;
    auto& [dx, dy] = delta_pos;
    return !(dx > 1 && dy > 1);
}

}  // namespace


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
        auto path = find_path_2d<position_t, navigator_t<position_t>>(
            e_pos, target_pos, 20);
        if(path->success && !path->steps.empty()) {
            auto next_step = path->steps.front();
            path->steps.pop_front();
            auto& map                  = engine::get_map();
            constexpr double atk_range = 2.1;
            auto distance_to_target    = distance2d_squared(
                e_pos.first, e_pos.second, target_pos.first, target_pos.second);
            if(distance_to_target <= atk_range) {
                attack(e_ent, engine::player);
            }
            else if(map.rect.contains(next_step)) {
                move_wait_attack(e_ent, next_step);
            }
        }
    }
}

// uses being, position, and viewshed
void ai_enemy() {
    using engine::player;
    using engine::reg;
    using namespace world;

    auto v_enemies  = reg.group<being_t, position_t, viewshed_t>();
    auto player_pos = reg.get<position_t>(player);

    // auto player_is_surrounded = surrounded(player_pos);

    auto aifunc = [&](auto e_ent) {
        auto [e_pos, vshed] = v_enemies.get<position_t, viewshed_t>(e_ent);

        if(e_ent == engine::player) {
            return;
        }
        auto player_is_visible = vshed.visible_coordinates.contains(player_pos);
        //  found player
        if(player_is_visible && e_ent != player) {
            ai_enemy_find_path(e_ent, player_pos);
        }
        // can't see the player
        else {
            random_walk(e_ent, e_pos);
        }
    };
    std::for_each(std::execution::par_unseq, v_enemies.begin(), v_enemies.end(),
                  aifunc);
}

}  // namespace radl
