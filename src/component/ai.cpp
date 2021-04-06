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

bool surrounded(const position_t& pos) {
    auto& map = engine::get_map();
    for(int dx = -1; dx <= 1; ++dx) {
        for(int dy = -1; dy <= 1; ++dy) {
            if(dx == 0 && dy == 0) {
                continue;
            }
            position_t offset = {dx, dy};
            auto look_pos     = pos + offset;
            auto walkable     = map[look_pos].characteristics.walkable;
            auto can_walk     = !is_occupied(reg, look_pos) && walkable;
            if(can_walk) {
                return false;
            }
        }
    }
    return true;
}

bool is_near(const position_t& src_pos, const position_t& dst_pos) {
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
        auto path = find_path_2d<position_t, navigator<position_t>>(e_pos,
                                                                    target_pos);
        if(path->success && !path->steps.empty()) {
            auto next_step = path->steps.front();
            path->steps.pop_front();
            auto& map = engine::get_map();
            if(map.rect.contains(next_step)) {
                move_wait_attack(e_ent, next_step);
            }
        }
    }
}


void ai_enemy(entt::registry& reg) {
    using engine::player;
    using namespace world;

    auto g_enemies  = reg.group<being_t, position_t>();
    auto player_pos = reg.get<position_t>(player);

    std::vector<entity> entities_parallel;
    entities_parallel.reserve(16);

    auto player_is_surrounded = surrounded(player_pos);
    for(auto [e_ent, e_b, e_pos] : g_enemies.each()) {
        if(e_ent == engine::player) {
            continue;
        }
        // auto&& e_pos   = std::get<position_t>(ent_tuple);
        // auto&& e_ent   = std::get<entity>(ent_tuple);

        auto& e_vshed = reg.get<viewshed_t>(e_ent);
        auto pos      = std::find(std::begin(e_vshed.visible_coordinates),
                             std::end(e_vshed.visible_coordinates), player_pos);
        //  found player
        if(pos != std::end(e_vshed.visible_coordinates)
           && !player_is_surrounded) {
            entities_parallel.push_back(e_ent);
        }
        else if(is_near(e_pos, player_pos)) {
            entities_parallel.push_back(e_ent);
        }
        // can't see the player
        else if(!is_near(e_pos, player_pos)) {
            random_walk(e_ent, e_pos);
        }
    }

    auto func = [&](entity ent) {
        if(ent != player) {
            ai_enemy_find_path(ent, player_pos);
        }
    };


    std::for_each(std::execution::par, entities_parallel.begin(),
                  entities_parallel.end(), func);
}

}  // namespace radl
