#include <ranges>
#include <algorithm>
#include <memory>

#include "spdlog/spdlog.h"
#include "component/component.hpp"

#include "rltk/rltk.hpp"
#include "map.hpp"
#include "utils/utils.hpp"
#include "utils/rng.hpp"
#include "system/game_state.hpp"

#include <ranges>
namespace radl {

using namespace rltk::colors;

void fill(Map& map, const rect_t& rect, tile_property_t characteristics,
          const tile_type_t& tiletype) {
    auto xi = std::min(rect.x1, rect.x2);
    auto xf = xi + rect.width();
    auto yi = std::min(rect.y1, rect.y2);
    auto yf = yi + rect.height();
    for(int x = xi; x < xf; ++x) {
        for(int y = yi; y < yf; ++y) {
            auto& tile = map.at(x, y);
            tile.type  = tiletype;
            tile.props = characteristics;
        }
    }
}

void try_apply_room_to_map(Map& map, const rect_t& rect) {
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
    fill(map, rect,
         tile_property_t{
             true,
             true,
         },
         tile_type_t::floor);
    map.rooms.push_back(rect);
}

constexpr int max_rooms = 30 * 10;
constexpr int min_size  = 6;
constexpr int max_size  = 12;


void create_random_rooms(Map& map) {
    for(int i = 0; i < max_rooms; ++i) {
        auto& rect = map.rect;
        int w      = rng::rng.range(min_size, max_size);
        int h      = rng::rng.range(min_size, max_size);
        int x      = rng::rng.range(1, rect.x2 - w - 2);
        int y      = rng::rng.range(1, rect.y2 - h - 2);
        rect_t rect_new{x, y, x + w, y + h};
        try_apply_room_to_map(map, rect_new);
    }
}

void corridor_horizontal(Map& map, int x1, int x2, int y) {
    auto xi                    = std::min(x1, x2);
    auto xf                    = std::max(x1, x2);
    tile_property_t tile_chars = {true, true};
    auto ttype                 = tile_type_t::floor;
    fill(map, rect_t{xi, y, xf + 1, y + 1}, tile_chars, ttype);
}

void corridor_vertical(Map& map, int y1, int y2, int x) {
    auto yi                    = std::min(y1, y2);
    auto yf                    = std::max(y1, y2);
    tile_property_t tile_chars = {true, true};
    auto ttype                 = tile_type_t::floor;
    fill(map, rect_t{x, yi, x + 1, yf + 1}, tile_chars, ttype);
}

void make_corridors_between_rooms(Map& map) {
    // make corridor with adjacent pairs in rooms vector
    adjacent_pairs(map.rooms.begin(), map.rooms.end(),
                   [&](rect_t& r1, rect_t& r2) {
                       const auto& [px, py] = r1.center();
                       const auto& [nx, ny] = r2.center();
                       auto chance          = rng::rng.range(0, 1);
                       if(chance == 0) {
                           corridor_horizontal(map, px, nx, py);
                           corridor_vertical(map, py, ny, nx);
                       } else {
                           corridor_horizontal(map, px, nx, ny);
                           corridor_vertical(map, py, ny, px);
                       }
                   });
}

using entity_vec = std::vector<entt::entity>;
std::unique_ptr<entity_vec> active_entities_near_player
    = std::make_unique<entity_vec>();
/**
 * @brief Get the entities near the player
 *
 * @return std::vector<entt::entity>*
 */
entity_vec* get_entities_near_player() {
    return active_entities_near_player.get();
}

void query_alive_entities_near_player() {
    active_entities_near_player->clear();
    constexpr int max_size = 32;
    using rltk::console;
    auto pos       = reg.get<position_t>(player);
    auto& [px, py] = pos;
    position_t p1  = {px - max_size, py - max_size};
    position_t p2  = {px + max_size, py + max_size};

    rect_t active_rect = rect_create(p1, p2);

    reg.view<viewshed_t, position_t, renderable_t>(
           entt::exclude<dead_t, player_t>)
        .each([&active_rect](entt::entity ent, viewshed_t& vshed,
                             const position_t& e_pos, renderable_t& rend) {
            if(ent != player && active_rect.contains(e_pos)) {
                active_entities_near_player->push_back(ent);
            }
        });
}

// XXX change this in the future, now we are only using entities that blocks the
// path
bool is_occupied(position_t target_pos) {
    auto& ents_here = get_map().at(target_pos).entities_here;
    return std::ranges::any_of(ents_here, [&](auto& ent) {
        return reg.any_of<blocks_t>(ent);
    });
}

bool is_exit(position_t target_pos) {
    auto& map = get_map();
    return !is_occupied(target_pos) && map.at(target_pos).props.walkable;
}

void Map::init(const rect_t& rect) {
    this->rect = rect;
    auto area  = rect.area();
    tiles.resize(area);
}

std::vector<std::pair<position_t, int>>
Map::get_available_exits(const position_t& pos) const {
    std::vector<std::pair<position_t, int>> vec;
    vec.reserve(8);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            if(x == 0 && y == 0) {
                continue;
            }
            auto check_pos = (pos + position_t{x, y});
            if(rect.contains(check_pos) && is_exit(check_pos)) {
                vec.push_back(std::make_pair(check_pos, 1));
            }
        }
    }
    return vec;
}

void tile_t::remove_entity(entity ent) {
    auto [beg, end] = std::ranges::remove(entities_here, ent);
    entities_here.erase(beg, end);
}

void tile_t::insert_entity(entity ent) {
    entities_here.push_back(ent);
}

}  // namespace radl
