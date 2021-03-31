#include "component/renderable.hpp"
#include "component/component.hpp"
#include "component/position.hpp"
#include "component/viewshed.hpp"
#include "rltk/rltk.hpp"
#include "map.hpp"
#include "spdlog/spdlog.h"
#include "camera.hpp"
#include "system/visibility.hpp"


namespace radl {

namespace {

// XXX groups for multicomponents are faster than views

struct nav_helper {
    static int get_x(const position_t& pos) {
        return pos.first;
    }
    static int get_y(const position_t& pos) {
        return pos.second;
    }
    static position_t get_xy(const int& x, const int& y) {
        return position_t{x, y};
    }
};

void fov_update(entt::registry& reg, const position_t& ent_pos,
                world::viewshed_t& viewshed, world::Map& map) {
    using namespace world;
    // resets the visibility
    reg.clear<visible_t>();

    auto set_visibility = [&](position_t reveal_pos) {
        if(map.rect.contains(reveal_pos)) {
            auto ent = map.at(reveal_pos);
            reg.emplace_or_replace<visible_t>(ent);
            if(!reg.has<explored_t>(ent)) {
                reg.emplace_or_replace<explored_t>(ent);
            }
        }
    };

    auto is_transparent = [&](position_t check_pos) {
        if(map.rect.contains(check_pos)) {
            auto ent   = map.at(check_pos);
            auto chars = reg.get<tile_characteristics_t>(ent);
            return chars.transparent;
        }
        return false;
    };

    radl::visibility_sweep_2d<position_t, nav_helper>(
        ent_pos, viewshed.range, set_visibility, is_transparent);
}

}  // namespace

void camera_update(entt::registry& reg, entt::entity ent) {
    using rltk::console;
    console->clear();
    using namespace world;
    const auto& [player_pos, rend, viewshed]
        = reg.get<position_t, renderable_t, viewshed_t>(ent);
    const auto& [px, py] = player_pos;

    auto view_map = reg.view<world::Map>();
    if(view_map.empty()) {
        spdlog::error("entity with map vmap, doesn't exist");
        return;
    }
    auto& map = view_map.get(view_map.front());

    fov_update(reg, player_pos, viewshed, map);

    auto xi = px - console->term_width / 2;
    auto xf = px + console->term_width / 2;
    auto yi = py - console->term_height / 2;
    auto yf = py + console->term_height / 2;

    rect_t render_viewport = {
        0,
        0,
        console->term_width,
        console->term_height,
    };

    // print all explored
    for(int x = xi; x < xf; ++x) {
        for(int y = yi; y < yf; ++y) {
            if(!map.rect.contains({x, y})) {
                continue;
            }
            auto tile = map[{x, y}];
            if(!reg.has<explored_t>(tile)) {
                continue;
            }
            auto vch       = reg.get<vchar_t>(tile);
            vch.foreground = DARKEST_GREY;
            auto renderpos = position_t{x - xi, y - yi};
            console->set_char(renderpos.first, renderpos.second, vch);
        }
    }

    // render visible tiles
    auto visible_view = reg.view<tile_t, visible_t, position_t>();
    for(const auto& ent : visible_view) {
        auto [pos, tile, vch] = reg.get<position_t, tile_t, vchar_t>(ent);
        const auto& [x, y]    = pos;
        auto renderpos        = position_t{x - xi, y - yi};
        if(!render_viewport.contains(renderpos)) {
            continue;
        }
        auto vch_print = vch;
        color_t fg;
        if(tile.type == tile_type_t::wall) {
            fg = LimeGreen;
        }
        else if(tile.type == tile_type_t::floor) {
            fg = SkyBlue;
        }
        vch_print.foreground = fg;
        console->set_char(renderpos.first, renderpos.second, vch_print);

        // render all entities in this tile
        for(auto& tile_ent : tile.entities) {
            const auto& rend = reg.get<renderable_t>(tile_ent);
            console->set_char(renderpos.first, renderpos.second, rend.vchar);
        }


        // console->set_char(x - xi, y - yi, vch_print);
    }

    // // auto group = registry.group<position>(entt::get<velocity,
    // renderable>); const auto& enemy_view
    //     = reg.view<destructible_t, renderable_t, position_t>();
    // for(auto ent : enemy_view) {
    //     const auto& [pos, rend] = reg.get<position_t, renderable_t>(ent);
    //     const auto& [x, y]      = pos;
    //     auto renderpos          = position_t{x - xi, y - yi};
    //     if(!render_viewport.contains(renderpos)) {
    //         continue;
    //     }
    //     const auto& vch = rend.vchar;
    //     console->set_char(renderpos.first, renderpos.second, vch);
    // }


    // render player
    // rend.vchar.background = color_t;
    auto player_vch = rend.vchar;
    console->set_char(px - xi, py - yi, player_vch);
}

}  // namespace radl
