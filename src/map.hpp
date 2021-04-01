#pragma once

#include <memory>
#include <vector>
#include <list>
#include <set>

#include "rltk/rltk.hpp"

#include "utils/rect.hpp"
#include "component/vchar.hpp"
#include "entt/entity/registry.hpp"

namespace radl::world {

using namespace rltk::colors;

enum tile_type_t {
    wall,
    floor,
};

struct tile_characteristics_t {
    bool walkable    = false;
    bool transparent = false;
};
struct tile_t {
    tile_type_t type;
    // tile_characteristics_t characteristics;
    // list because I don't want realocations
    std::list<entt::entity> entities;
};

struct visible_t {};

struct explored_t {};

struct blocks_t {};



struct Map {
    rect_t rect;

    std::vector<rect_t> rooms;

    // cada posição no vetor é uma posição fixa no mapa
    // preciso arrumar um jeito depois das posições se auto ajustarem
    // cada entidade aqui vai apontar de volta pra posição em que ela ocupa
    // não tô vendo muita vantagem em continuar deixando as entidades aqui
    std::vector<entt::entity> tiles;

    [[nodiscard]] inline const entt::entity& operator[](position_t pos) const {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline entt::entity& operator[](position_t pos) {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline entt::entity& at(position_t pos) {
        auto& [x, y] = pos;
        return tiles[x + y * rect.width()];
    }

    inline entt::entity& at(int x, int y) {
        return tiles[x + y * rect.width()];
    }

    [[nodiscard]] inline const entt::entity& at(int x, int y) const {
        return tiles[x + y * rect.width()];
    }

    static inline Map& get_map(entt::registry& reg) {
        auto map_view = reg.view<Map>();
        auto& map     = map_view.get(map_view.front());
        return map;
    }


    static inline tile_t& get_tile(entt::registry& reg, Map& map,
                                   position_t pos) {
        auto& tent = map.at(pos);
        auto& tile = reg.get<tile_t>(tent);
        return tile;
    }

    static inline tile_t& get_tile(entt::registry& reg, entt::entity tile_ent,
                                   position_t pos) {
        auto& tile = reg.get<tile_t>(tile_ent);
        return tile;
    }

    static inline tile_t& get_tile(entt::registry& reg, position_t pos) {
        auto& map  = get_map(reg);
        auto& tent = map.at(pos);
        auto& tile = reg.get<tile_t>(tent);
        return tile;
    }

    inline void init(entt::registry& reg, const rect_t& r) {
        // m_reg     = reg;
        rect      = r;
        auto area = r.area();
        tiles.resize(area);

        for(int x = 0; x < r.width(); ++x) {
            for(int y = 0; y < r.height(); ++y) {
                auto tile                = reg.create();
                tiles[x + y * r.width()] = tile;
                reg.emplace<position_t>(tile, position_t{x, y});
                reg.emplace<tile_t>(tile, tile_type_t::wall);
                reg.emplace<tile_characteristics_t>(tile);
                reg.emplace<vchar_t>(tile, vchar_t{
                                               glyph::SOLID1,
                                               WHITE,
                                               BLACK,
                                           });
            }
        }
    }
};


auto is_occupied(entt::registry& reg, position_t target_pos) -> bool;

Map make_test_map(const rect_t& dimension, const position_t& player_pos);

Map new_map(entt::registry& reg, const rect_t& rect);

}  // namespace radl::world
