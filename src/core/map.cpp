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


using namespace rltk::colors;

void fill(Map& map, const rect_t& rect, tile_characteristics_t characteristics,
          const tile_type_t& tiletype) {
    auto xi = std::min(rect.x1, rect.x2);
    auto xf = xi + rect.width();
    auto yi = std::min(rect.y1, rect.y2);
    auto yf = yi + rect.height();
    for(int x = xi; x < xf; ++x) {
        for(int y = yi; y < yf; ++y) {
            auto& tile           = map.at(x, y);
            tile.type            = tiletype;
            tile.characteristics = characteristics;
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
         tile_characteristics_t{
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
    auto xi                           = std::min(x1, x2);
    auto xf                           = std::max(x1, x2);
    tile_characteristics_t tile_chars = {true, true};
    auto ttype                        = tile_type_t::floor;
    fill(map, rect_t{xi, y, xf + 1, y + 1}, tile_chars, ttype);
}

void corridor_vertical(Map& map, int y1, int y2, int x) {
    auto yi                           = std::min(y1, y2);
    auto yf                           = std::max(y1, y2);
    tile_characteristics_t tile_chars = {true, true};
    auto ttype                        = tile_type_t::floor;
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
                       }
                       else {
                           corridor_horizontal(map, px, nx, ny);
                           corridor_vertical(map, py, ny, px);
                       }
                   });
}


// std::unique_ptr<Map> new_map(const rect_t& rect) {
//     std::unique_ptr<Map> map = std::make_unique<Map>();
//     map->init(rect);
//     create_random_rooms(*map.get());
//     make_corridors_between_rooms(*map.get());
//     return map;
// }

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

void query_alive_entities_near_player() {
    active_entities_near_player->clear();
    constexpr int max_size = 32;
    using engine::reg;
    using rltk::console;
    auto pos       = reg.get<position_t>(engine::player);
    auto& [px, py] = pos;
    position_t p1  = {px - max_size, py - max_size};
    position_t p2  = {px + max_size, py + max_size};

    rect_t active_rect = rect_create(p1, p2);

    reg.view<viewshed_t, position_t, renderable_t>(
           entt::exclude<dead_t, player_t>)
        .each([&active_rect](entt::entity ent, viewshed_t& vshed,
                             const position_t& e_pos, renderable_t& rend) {
            if(ent != engine::player && active_rect.contains(e_pos)) {
                active_entities_near_player->push_back(ent);
            }
        });
}

// XXX change this in the future, now we are only using entities that blocks the
// path
bool is_occupied(position_t target_pos) {
    using engine::reg;
    auto& ents_here = engine::get_map().at(target_pos).entities_here;
    return std::ranges::any_of(ents_here, [&](auto& ent) {
        return reg.any_of<blocks_t>(ent);
    });
}

// void map_entity_walk(entity ent, const position_t& src_pos,
//                      const position_t& dst_pos) {
//     using engine::reg;
//     auto& map = engine::get_map();
//     map[src_pos].entities_here.remove(ent);
//     map[dst_pos].entities_here.push_back(ent);
//     reg.patch<position_t>(ent, [=](position_t& pos) {
//         pos = dst_pos;
//     });
// }

void Map::init(const rect_t& rect) {
    this->rect = rect;
    auto area  = rect.area();
    tiles.resize(area);
}

// Map::~Map() {
//     tiles.clear();
//     rooms.clear();
// }

// tile_t::~tile_t() {
//     entities_here.clear();
// }

}  // namespace radl::world
