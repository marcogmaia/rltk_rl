#pragma once

#include <memory>
#include <vector>
#include <list>
#include <set>

#include "entt/entity/registry.hpp"

#include "utils/rect.hpp"
#include "component/vchar.hpp"


namespace radl {
using namespace component;

using namespace rltk::colors;
using entt::entity;
using entt::registry;

enum tile_type_t {
    wall,
    floor,
};

struct tile_property_t {
    bool walkable    = false;
    bool transparent = false;
    bool explored    = false;
    // bool is_visible  = false;
};

struct blocks_t {};

// struct active_t {};

// struct explored_t {};
enum tile_status_t {
    NONE,
    BLOODIED,
};

struct tile_t {
    tile_type_t type = wall;
    tile_property_t props{};
    tile_status_t status = tile_status_t::NONE;
    std::list<entity> entities_here{};

    // ~tile_t();

    inline vchar_t get_vchar() const {
        vchar_t vch = {
            '\0',
            DARKEST_GREY,
            BLACK,
        };

        switch(type) {
        case tile_type_t::wall: vch.glyph = glyph::SOLID1; break;
        case tile_type_t::floor: vch.glyph = glyph::BLOCK1; break;
        default: break;
        }
        return vch;
    }

    void remove_entity(entity ent);
    void insert_entity(entity ent);
};

// struct visible_t {};


struct Map {
    rect_t rect;

    std::vector<rect_t> rooms;
    std::vector<tile_t> tiles;

    [[nodiscard]] inline const tile_t& operator[](position_t pos) const {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline tile_t& operator[](position_t pos) {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline tile_t& at(position_t pos) {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline tile_t& at(int x, int y) {
        return tiles[x + y * rect.width()];
    }

    [[nodiscard]] inline tile_t at(int x, int y) const {
        return tiles[x + y * rect.width()];
    }

    void init(const rect_t& rect);

    std::vector<std::pair<position_t, int>>
    get_available_exits(const position_t& pos) const;
};

void create_random_rooms(Map& map);

void make_corridors_between_rooms(Map& map);

bool is_occupied(position_t target_pos);

Map make_test_map(const rect_t& dimension, const position_t& player_pos);

// Map new_map(entt::registry& reg, const rect_t& rect);
std::unique_ptr<Map> new_map(registry& reg, const rect_t& rect);

std::vector<entt::entity>* get_entities_near_player();

void query_alive_entities_near_player();
// void update_enemies_visibility(entt::registry& reg, entt::entity ent);


template <typename C1, typename C2>
void move_append(C1& src, C2& dst) {
    if(dst.empty()) {
        dst = std::move(src);
    } else {
        dst.reserve(dst.size() + src.size());
        std::move(std::begin(src), std::end(src), std::back_inserter(dst));
        src.clear();
    }
}
// void map_entity_walk(entity ent, const position_t& src_pos,
//                      const position_t& dst_pos);


}  // namespace radl
