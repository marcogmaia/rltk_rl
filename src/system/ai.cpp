#include <algorithm>
#include <execution>
#include <memory>
#include <ranges>

#include "component/combat.hpp"
#include "component/enemy.hpp"
#include "component/viewshed.hpp"
#include "component/movable.hpp"

#include "core/game_state.hpp"
#include "core/map/dijkstra_map.hpp"

#include "system/ai.hpp"
#include "system/player_action.hpp"

#include "utils/path_finding.hpp"

namespace radl::system {


namespace {

[[maybe_unused]] bool surrounded(const position_t& pos) {
    auto& map = engine::get_map();
    for(int dx = -1; dx <= 1; ++dx) {
        for(int dy = -1; dy <= 1; ++dy) {
            if(dx == 0 && dy == 0) {
                continue;
            }
            position_t offset = {dx, dy};
            auto look_pos     = pos + offset;
            auto walkable     = map[look_pos].props.walkable;
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
    auto [dx, dy] = dst_pos - src_pos;
    return !(dx > 1 && dy > 1);
}

}  // namespace


void ai_enemy_find_path(entity e_ent, const position_t& target_pos) {
    // FIXME this must be a system
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

            auto distance_to_target = distance2d_squared(
                e_pos.first, e_pos.second, target_pos.first, target_pos.second);
            if(distance_to_target <= atk_range) {
                attack(e_ent, player);
            } else if(map.rect.contains(next_step)) {
                move_wait_attack(e_ent, next_step);
            }
        }
    }
}

// uses being, position, and viewshed
void ai_enemy_astar() {
    auto player_pos    = reg.get<position_t>(player);
    auto& enemies_near = *get_entities_near_player();
    // function to apply to each enemy
    auto aifunc = [&](auto e_ent) {
        if(!reg.any_of<enemy_t>(e_ent)) {
            return;
        }
        auto [e_pos, vshed, enemy]
            = reg.get<position_t, viewshed_t, enemy_t>(e_ent);

        auto player_is_visible = vshed.visible_coordinates.contains(player_pos);
        //  found player
        if(player_is_visible) {
            ai_enemy_find_path(e_ent, player_pos);
            enemy.remembers_target_position = true;
            enemy.memory                    = player_pos;
            enemy.remember_turns            = 5;
        }
        // can't see the player
        else {
            random_walk(reg, e_ent, e_pos);
        }
    };
    std::for_each(std::execution::par_unseq, enemies_near.begin(),
                  enemies_near.end(), aifunc);
}


void ai_enemy_dijkstra_map(entity ent) {
    auto& dm            = reg.ctx<DijkstraMap>();
    const auto& ent_pos = reg.get<position_t>(ent);
    // todo pick random minimum position
    auto [found, target_pos] = dm.find_lowest_path(ent_pos);
    auto player_pos          = reg.get<position_t>(player);

    bool somebody_already_wants_to_walk_to = false;
    // check if anybody already wants to walk to this tile
    for(auto [e_other, other_walk] : reg.view<want_to_walk_t>().each()) {
        if(other_walk.to == target_pos) {
            // random walk
            somebody_already_wants_to_walk_to = true;
        }
    }
    // if player is next to ent: attack // distance to player is 1
    if(distance_pythagoras(player_pos, ent_pos) < 1.5) {
        // attack player
        attack(ent, player);
    }
    // if not found a valid target position: random_walk
    else if(!found) {
        random_walk(reg, ent, ent_pos);
        return;
    } else if(!somebody_already_wants_to_walk_to) {
        want_to_walk_t want_walk = {ent_pos, target_pos};
        reg.emplace<want_to_walk_t>(ent, want_walk);
    }
}


}  // namespace radl::system
