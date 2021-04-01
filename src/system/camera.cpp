#include "component/renderable.hpp"
#include "component/component.hpp"
#include "component/position.hpp"
#include "component/viewshed.hpp"
#include "rltk/rltk.hpp"
#include "map.hpp"
#include "spdlog/spdlog.h"
#include "camera.hpp"
#include "system/visibility.hpp"

#include <ranges>


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
    // dont need a viewshed with positions, when can get all visible_t
    // componenets, and they have the entity have the position

    auto set_visibility = [&](position_t reveal_pos) {
        if(map.rect.contains(reveal_pos)) {
            auto ent = map.at(reveal_pos);
            reg.emplace_or_replace<visible_t>(ent);
            // viewshed.visible_coordinates.push_back(reveal_pos);
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
    // auto& map = view_map.get(view_map.front());
    auto& map = reg.get<world::Map>(engine::map);

    fov_update(reg, player_pos, viewshed, map);

    auto xci = px - console->term_width / 2;
    auto xcf = px + console->term_width / 2;
    auto yci = py - console->term_height / 2;
    auto ycf = py + console->term_height / 2;

    rect_t render_viewport = {
        0,
        0,
        console->term_width,
        console->term_height,
    };

    auto render_pos = [&](int xr, int yr) -> position_t {
        return position_t{xr - xci, yr - yci};
    };

    auto update_vicinity = [&]() -> bool {
        constexpr int max_size = 32;
        position_t p1          = {px - max_size, py - max_size};
        position_t p2          = {px + max_size, py + max_size};
        auto& [xui, yui]       = p1;
        auto& [xuf, yuf]       = p2;

        for(int x = xui; x < xuf; ++x) {
            for(int y = yui; y < yuf; ++y) {
                // render only valid and visible positions
                if(!map.rect.contains({x, y})
                   || !reg.has<visible_t>(map[{x, y}])) {
                    continue;
                }
                auto tile = map.get_tile(reg, map, position_t{x, y});
                for(auto& et : tile.entities) {
                    auto rend = reg.try_get<renderable_t>(et);
                    if(!rend) {
                        continue;
                    }
                    // auto abs_rpos = reg.get<position_t>(et);
                    auto&& rpos = render_pos(x, y);
                    console->set_char(rpos.first, rpos.second, rend->vchar);
                }
            }
        }
        return true;
    };

    // print all explored
    for(int x = xci; x < xcf; ++x) {
        for(int y = yci; y < ycf; ++y) {
            // if(!should_update({x,y})) continue;
            if(!map.rect.contains({x, y})) {
                continue;
            }
            auto tile = map[{x, y}];
            if(!reg.has<explored_t>(tile)) {
                continue;
            }
            auto vch       = reg.get<vchar_t>(tile);
            vch.foreground = DARKEST_GREY;
            auto renderpos = render_pos(x, y);
            console->set_char(renderpos.first, renderpos.second, vch);
        }
    }
    // if(in updatable range)
    // if pos is visible print pos
    // pick the highest z level entity in tile and print


    // render visible tiles
    auto visible_view = reg.view<tile_t, position_t, vchar_t, visible_t>();
    for(auto ent : visible_view) {
        auto [tile, pos, vch]
            = visible_view.get<tile_t, position_t, vchar_t>(ent);
        auto [x, y]    = pos;
        auto renderpos = position_t{x - xci, y - yci};
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


        console->set_char(x - xci, y - yci, vch_print);
    }


    update_vicinity();

    // auto enemy_view = reg.view<enemy_t, position_t, renderable_t>();
    // for(auto ent : enemy_view) {
    //     auto& rend      = enemy_view.get<renderable_t>(ent);
    //     auto& enemy_pos = enemy_view.get<position_t>(ent);
    //     auto [x, y]     = enemy_pos;
    //     auto contains   = std::ranges::any_of(viewshed.visible_coordinates,
    //                                         [&](const position_t& v_pos) {
    //                                             return enemy_pos == v_pos;
    //                                         });
    //     if(contains) {
    //         auto renderpos = position_t{x - xi, y - yi};
    //         if(render_viewport.contains(renderpos)) {
    //             console->set_char(renderpos.first, renderpos.second,
    //                               rend.vchar);
    //         }
    //     }
    // }


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
    console->set_char(px - xci, py - yci, player_vch);
}

}  // namespace radl
