#pragma once

#include <memory>
#include <vector>
#include <list>
#include <set>

#include "utils/rect.hpp"
#include "entt/entity/registry.hpp"
#include "component/vchar.hpp"

namespace radl::world {

namespace {

using namespace rltk::colors;

}

enum tile_type_t {
    wall,
    floor,
};

struct tile_characteristics_t {
    bool walkable    = false;
    bool transparent = false;
    bool explored    = false;
};

struct active_t {};

// struct explored_t {};

struct blocks_t {};

struct tile_t {
    tile_type_t type;
    tile_characteristics_t characteristics;

    inline vchar_t get_vchar() const {
        vchar_t vch = {
            '\0',
            DARKEST_GREY,
            BLACK,
        };

        switch(type) {
        case tile_type_t::wall:
            vch.glyph = glyph::SOLID1;
            break;
        case tile_type_t::floor:
            vch.glyph = glyph::BLOCK1;
            break;
        default:
            break;
        }
        return vch;
    }
    // position_t position;
    // tile_characteristics_t characteristics;
    // list because I don't want realocations
    // std::list<entt::entity> entities;
};

// struct visible_t {};


struct Map {
    rect_t rect;

    std::vector<rect_t> rooms;

    // cada posição no vetor é uma posição fixa no mapa
    // preciso arrumar um jeito depois das posições se auto ajustarem
    // cada entidade aqui vai apontar de volta pra posição em que ela ocupa
    // não tô vendo muita vantagem em continuar deixando as entidades aqui
    // FIXME replace entity for tile_t, the position will no longer contain the
    // entities
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

    [[nodiscard]] inline const tile_t at(int x, int y) const {
        return tiles[x + y * rect.width()];
    }

    inline void init(entt::registry& reg, const rect_t& r) {
        rect      = r;
        auto area = r.area();
        tiles.resize(area);

        tile_t initial_tile{
            .type = tile_type_t::wall,
            .characteristics{
                false,
                false,
            },
        };

        for(int x = 0; x < r.width(); ++x) {
            for(int y = 0; y < r.height(); ++y) {
                // auto tile                = reg.create();
                tiles[x + y * r.width()] = initial_tile;
                // reg.emplace<position_t>(tile, position_t{x, y});
                // reg.emplace<tile_t>(tile, tile_type_t::wall);
                // reg.emplace<tile_characteristics_t>(tile);
                // reg.emplace<vchar_t>(tile, vchar_t{
                //                                glyph::SOLID1,
                //                                WHITE,
                //                                BLACK,
                //                            });
            }
        }
    }
};


bool is_occupied(entt::registry& reg, position_t target_pos);

Map make_test_map(const rect_t& dimension, const position_t& player_pos);

Map new_map(entt::registry& reg, const rect_t& rect);

std::vector<entt::entity>* get_entities_near_player();

void query_entities_near_player();
// void update_enemies_visibility(entt::registry& reg, entt::entity ent);
}  // namespace radl::world
