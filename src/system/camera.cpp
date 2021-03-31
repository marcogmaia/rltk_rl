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

void fov_update(const position_t& ent_pos, world::viewshed_t& vshed,
                world::Map& map) {
    using namespace world;
    // resets the visibility
    map.reg.clear<visible_t>();

    auto set_visibility = [&](position_t reveal_pos) {
        if(map.rect.contains(reveal_pos)) {
            auto ent = map.at(reveal_pos);
            map.reg.emplace_or_replace<visible_t>(ent);
            if(!map.reg.has<explored_t>(ent)) {
                map.reg.emplace_or_replace<explored_t>(ent);
            }
        }
    };

    auto is_transparent = [&](position_t check_pos) {
        if(map.rect.contains(check_pos)) {
            auto ent   = map.at(check_pos);
            auto chars = map.reg.get<tile_characteristics_t>(ent);
            return chars.transparent;
        }
        return false;
    };

    radl::visibility_sweep_2d<position_t, nav_helper>(
        ent_pos, vshed.range, set_visibility, is_transparent);
}

}  // namespace

void camera_update(entt::registry& reg, entt::entity ent) {
    using rltk::console;
    console->clear();
    using namespace world;
    const auto& [player_pos, rend, vshed]
        = reg.get<position_t, renderable_t, viewshed_t>(ent);
    const auto& [px, py] = player_pos;

    auto ent_map = reg.view<world::Map>().front();
    if(!reg.valid(ent_map)) {
        spdlog::error("entity with map ent_map, doesn't exist");
        return;
    }
    auto& map = reg.get<world::Map>(ent_map);

    fov_update(player_pos, vshed, map);

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
            if(!map.reg.has<explored_t>(tile)) {
                continue;
            }
            auto vch       = map.reg.get<vchar_t>(tile);
            auto renderpos = position_t{x - xi, y - yi};
            console->set_char(renderpos.first, renderpos.second, vch);
        }
    }

    // render visible tiles
    auto visible_view = map.reg.view<tile_t, visible_t, position_t>();
    for(auto& ent : visible_view) {
        auto [pos, ttype, vch] = map.reg.get<position_t, tile_t, vchar_t>(ent);
        const auto& [x, y]     = pos;
        auto renderpos         = position_t{x - xi, y - yi};
        if(!render_viewport.contains(renderpos)) {
            continue;
        }
        auto vch_print = vch;
        color_t fg;
        if(ttype.type == tile_type_t::wall) {
            fg = LimeGreen;
        }
        else if(ttype.type == tile_type_t::floor) {
            fg = SkyBlue;
        }
        vch_print.foreground = fg;
        console->set_char(renderpos.first, renderpos.second, vch_print);
        // console->set_char(x - xi, y - yi, vch_print);
    }

    // auto enemy_view = reg.view<position_t, renderable_t>();
    // for(auto ent : enemy_view) {
    //     const auto& [pos, dstb]
    //         = reg.get<position_t, renderable_t>(ent);
    //     auto& vch = rend.vchar;
    //     if(map.visible_coordinates.contains(pos)) {
    //         console->set_char(px - xi, py - yi, vch);
    //     }
    // }


    // render player
    // rend.vchar.background = color_t;
    auto player_vch = rend.vchar;
    console->set_char(px - xi, py - yi, player_vch);
}

}  // namespace radl
