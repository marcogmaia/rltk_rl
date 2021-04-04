#include <algorithm>
#include <ranges>
#include <execution>
#include <memory>
#include "component/component.hpp"
#include "core/map.hpp"
#include "core/engine.hpp"
#include "system/player_action.hpp"

#include "utils/path_finding.hpp"
#include "utils/geometry.hpp"

namespace radl {

namespace {

using location_t = position_t;


}  // namespace

// TODO try to make my own concept (C++20 concepts) for the navigator
struct navigator {
    static int get_x(const location_t& pos) {
        return pos.first;
    }
    static int get_y(const location_t& pos) {
        return pos.second;
    }
    static location_t get_xy(int x, int y) {
        return location_t{x, y};
    }
    static bool is_walkable(const location_t& pos) {
        auto& map = engine::reg.ctx<world::Map>();
        return map.rect.contains(pos) && map[pos].characteristics.walkable;
    }
    // This lets you define a distance heuristic. Manhattan distance works
    // really well, but for now we'll just use a simple euclidian distance
    // squared. The geometry system defines one for us.
    static double get_distance_estimate(location_t& pos, location_t& goal) {
        auto& [xi, yi] = pos;
        auto& [xf, yf] = goal;
        float d        = distance2d_squared(xi, yi, xf, yf);
        return d;
    }

    // Heuristic to determine if we've reached our destination? In some cases,
    // you'd not want this to be a simple comparison with the goal - for
    // example, if you just want to be adjacent to (or even a preferred distance
    // from) the goal. In this case, we're trying to get to the goal rather than
    // near it.
    static bool is_goal(location_t& pos, location_t& goal) {
        return pos == goal;
    }

    // This is where we calculate where you can go from a given tile. In this
    // case, we check all 8 directions, and if the destination is walkable
    // return it as an option.
    static bool get_successors(location_t pos,
                               std::vector<location_t>& successors) {
        for(int x = -1; x <= 1; ++x) {
            for(int y = -1; y <= 1; ++y) {
                if(x == 0 && y == 0) {
                    continue;
                }
                location_t offset{x, y};
                auto w_pos = pos + offset;
                auto map   = engine::reg.ctx<world::Map>();
                if(map.rect.contains(w_pos)
                   && map[w_pos].characteristics.walkable) {
                    successors.push_back(w_pos);
                }
            }
        }
        return true;
    }

    // This function lets you set a cost on a tile transition. For now, we'll
    // always use a cost of 1.0.
    static double get_cost(location_t& position, location_t& successor) {
        return 1.0;
    }

    // This is a simple comparison to determine if two locations are the same.
    // It just passes through to the location_t's equality operator in this
    // instance (we didn't do that automatically) because there are times you
    // might want to behave differently.
    static bool is_same_state(const location_t& lhs, const location_t& rhs) {
        return lhs == rhs;
    }
};


void ai_enemy_find_path(entity e_ent, const position_t& target_pos) {
    using namespace world;
    using engine::reg;
    viewshed_t& e_vshed = reg.get<viewshed_t>(e_ent);
    auto& vis_coords    = e_vshed.visible_coordinates;

    auto found_at = std::ranges::find(vis_coords, target_pos);
    // found
    if(found_at != vis_coords.end()) {
        auto e_pos = reg.get<position_t>(e_ent);
        if(e_pos != target_pos) {
            auto path = find_path_2d<position_t, navigator>(e_pos, target_pos);
            if(path->success && path->steps.size() > 1) {
                auto next_step = path->steps.front();
                path->steps.pop_front();
                auto& map = reg.ctx<Map>();
                if(map.rect.contains(next_step)) {
                    walk(e_ent, next_step);
                }
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
        if(ent != player) {
            ai_enemy_find_path(ent, player_pos);
        }
    };

    std::for_each(std::execution::par_unseq, g_enemies.begin(), g_enemies.end(),
                  func);
}

}  // namespace radl
