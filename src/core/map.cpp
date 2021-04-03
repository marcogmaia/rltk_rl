#include <ranges>
#include <algorithm>
#include <memory>

#include "spdlog/spdlog.h"
#include "component/component.hpp"

#include "rltk/rltk.hpp"
#include "map.hpp"
#include "utils/utils.hpp"
#include "utils/rng.hpp"
#include "core/engine.hpp"

#include <ranges>
namespace radl::world {

namespace {

using namespace rltk::colors;

void fill(entt::registry& reg, Map& map, const rect_t& rect,
          tile_characteristics_t interaction, const tile_type_t& tiletype) {
    auto xi = std::min(rect.x1, rect.x2);
    auto xf = xi + rect.width();
    auto yi = std::min(rect.y1, rect.y2);
    auto yf = yi + rect.height();
    for(int x = xi; x < xf; ++x) {
        for(int y = yi; y < yf; ++y) {
            auto& tile = map.at(x, y);
            tile       = {tiletype, interaction};
        }
    }
}

void try_apply_room_to_map(entt::registry& reg, Map& map, const rect_t& rect) {
    spdlog::debug("Applying room:\nr1: {}\nr2: {}", map.rect.to_string(),
                  rect.to_string());
    // ## check if dimensions are valid
    if(!rect.is_inside(map.rect)) {
        spdlog::debug("apply_room: Rect not completely inside.");
        return;
    }
    // quadratic algorithm, kinda dumb
    // ## check if overlap with existing rooms
    for(const auto& other_room : map.rooms) {
        if(rect.intersect(other_room)) {
            spdlog::debug("room intersects\n");
            return;
        }
    }
    // ## if everything is ok, create room
    fill(reg, map, rect,
         tile_characteristics_t{
             true,
             true,
         },
         tile_type_t::floor);
    map.rooms.push_back(rect);
}

constexpr int max_rooms = 30;
constexpr int min_size  = 6;
constexpr int max_size  = 12;


void create_random_rooms(entt::registry& reg, Map& map) {
    for(int i = 0; i < max_rooms; ++i) {
        auto& rect = map.rect;
        int w      = rng::rng.range(min_size, max_size);
        int h      = rng::rng.range(min_size, max_size);
        int x      = rng::rng.range(1, rect.x2 - w - 2);
        int y      = rng::rng.range(1, rect.y2 - h - 2);
        rect_t rect_new{x, y, x + w, y + h};
        try_apply_room_to_map(reg, map, rect_new);
    }
}

void corridor_horizontal(entt::registry& reg, Map& map, int x1, int x2, int y) {
    auto xi                           = std::min(x1, x2);
    auto xf                           = std::max(x1, x2);
    tile_characteristics_t tile_chars = {true, true};
    auto ttype                        = tile_type_t::floor;
    fill(reg, map, rect_t{xi, y, xf + 1, y + 1}, tile_chars, ttype);
}

void corridor_vertical(entt::registry& reg, Map& map, int y1, int y2, int x) {
    auto yi                           = std::min(y1, y2);
    auto yf                           = std::max(y1, y2);
    tile_characteristics_t tile_chars = {true, true};
    auto ttype                        = tile_type_t::floor;
    fill(reg, map, rect_t{x, yi, x + 1, yf + 1}, tile_chars, ttype);
}

void make_corridors_between_rooms(entt::registry& reg, Map& map) {
    // make corridor with adjacent pairs in rooms vector
    adjacent_pairs(map.rooms.begin(), map.rooms.end(),
                   [&](rect_t& r1, rect_t& r2) {
                       const auto& [px, py] = r1.center();
                       const auto& [nx, ny] = r2.center();
                       auto chance          = rng::rng.range(0, 1);
                       if(chance == 0) {
                           corridor_horizontal(reg, map, px, nx, py);
                           corridor_vertical(reg, map, py, ny, nx);
                       }
                       else {
                           corridor_horizontal(reg, map, px, nx, ny);
                           corridor_vertical(reg, map, py, ny, px);
                       }
                   });
}

}  // namespace

Map new_map(entt::registry& reg, const rect_t& rect) {
    Map map;
    map.init(reg, rect);
    create_random_rooms(reg, map);
    make_corridors_between_rooms(reg, map);
    return map;
}

std::unique_ptr<std::vector<entt::entity>> active_entities_near_player
    = std::make_unique<std::vector<entt::entity>>();
/**
 * @brief Get the entities near the player
 *
 * @return std::vector<entt::entity>*
 */
std::vector<entt::entity>* get_entities_near_player() {
    return active_entities_near_player.get();
}

void query_entities_near_player() {
    active_entities_near_player->clear();
    constexpr int max_size = 32;
    using engine::reg;
    using rltk::console;
    auto pos       = reg.get<position_t>(engine::player);
    auto& [px, py] = pos;
    position_t p1  = {px - max_size, py - max_size};
    position_t p2  = {px + max_size, py + max_size};

    rect_t active_rect = rect_create(p1, p2);

    reg.view<viewshed_t, position_t, renderable_t>().each(
        [&active_rect](entt::entity ent, viewshed_t& vshed,
                       const position_t& e_pos, renderable_t& rend) {
            if(active_rect.contains(e_pos)) {
                active_entities_near_player->push_back(ent);
            }
        });
}


bool is_occupied(entt::registry& reg, position_t target_pos) {
    return std::ranges::any_of(*active_entities_near_player.get(),
                               [&](auto& ent) {
                                   auto active_pos = reg.get<position_t>(ent);
                                   return active_pos == target_pos;
                               });
}


}  // namespace radl::world
