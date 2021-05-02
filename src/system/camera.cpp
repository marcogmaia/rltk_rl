#include <ranges>
#include <algorithm>
#include <execution>

#include "spdlog/spdlog.h"
#include "rltk/rltk.hpp"

#include "core/gui/gui.hpp"
#include "system/camera.hpp"
#include "system/game_state.hpp"
#include "system/map/map.hpp"

#include "system/map/dijkstra_map.hpp"


namespace radl::system {

// TODO make option in GOD ui to show the entire map and change camera position
namespace {

position_t render_position(const position_t& pos, const camera_t& camera) {
    auto& [ox, oy] = pos;
    auto& [cx, cy] = camera.position;
    auto x_off     = cx - camera.width / 2;
    auto y_off     = cy - camera.height / 2;
    return position_t{ox - x_off, oy - y_off};
}

}  // namespace

void camera() {
    auto& camera = reg.ctx<camera_t>();
    if(!camera.dirty) {
        return;
    }
    camera.dirty = false;

    auto& [cx, cy] = camera.position;
    auto xci       = cx - camera.width / 2;
    auto yci       = cy - camera.height / 2;
    auto xcf       = cx + camera.width / 2;
    auto ycf       = cy + camera.height / 2;
    // rect_t render_rect = {xci, yci, xcf, ycf};
    auto& base_map = reg.ctx<Map>();

    term(gui::UI_MAP)->clear();
    term(gui::UI_ENTITIES)->clear();
    // render all explored
    for(int x = xci; x < xcf; ++x) {
        for(int y = yci; y < ycf; ++y) {
            // if(!should_update({x,y})) continue;
            if(!base_map.rect.contains({x, y})) {
                continue;
            }
            const auto& tile = base_map.at({x, y});
            if(!tile.props.explored) {
                continue;
            }
            auto vch       = tile.get_vchar();
            vch.foreground = color_t(15, 15, 15);
            // vch.foreground = DARKEST_GREY;
            auto [rx, ry] = render_position(position_t{x, y}, camera);
            term(gui::UI_MAP)->set_char(rx, ry, vch);
        }
    }

    // render map visible
    auto& pvshed = reg.get<viewshed_t>(player);
    for(const auto& v_pos : pvshed.visible_coordinates) {
        const auto& tile = base_map.at(v_pos);
        auto [rx, ry]    = render_position(v_pos, camera);
        vchar_t tile_vch = tile.get_vchar();
        if(tile.type == tile_type_t::wall) {
            tile_vch.foreground = color_t(0, 31, 36);
        } else if(tile.type == tile_type_t::floor) {
            if(tile.status == tile_status_t::BLOODIED) {
                tile_vch.foreground = DARKER_RED;
            } else {
                tile_vch.foreground = color_t(36, 18, 4);
            }
            term(gui::UI_ENTITIES)
                ->set_char(rx, ry, vchar_t{glyph::CDOT, DARK_GREY, BLACK});
        }

        term(gui::UI_MAP)->set_char(rx, ry, tile_vch);

        // auto& dm         = reg.ctx<DijkstraMap>();
        // auto cost        = static_cast<int>(dm.at(v_pos));
        // vchar_t vch_cost = {glyph::SOLID1,
        //                     color_t{
        //                         cost * 8,
        //                         cost * 8,
        //                         cost * 8,
        //                     },
        //                     BLACK};
        // term(gui::UI_MAP)->set_char(rx, ry, vch_cost);
    }

    // render renderables
    // only needs to sort when a new renderable is added or removed
    reg.sort<renderable_t>(
        [](const renderable_t& lhs, const renderable_t& rhs) {
            return lhs.z_level < rhs.z_level;
        });
    // reg.sor
    for(auto [ent, rend] : reg.view<renderable_t>().each()) {
        if(!reg.any_of<position_t>(ent)) {
            continue;
        }
        const auto& rpos = reg.get<position_t>(ent);
        if(!pvshed.visible_coordinates.contains(rpos)) {
            continue;
        }
        auto [rx, ry] = render_position(rpos, camera);
        term(gui::UI_ENTITIES)->set_char(rx, ry, rend.vchar);
    }

    // render particles
    rltk::sterm(gui::UI_PARTICLES)->clear();
    for(auto [ent, rend, pos, particle] :
        reg.view<renderable_t, position_t, particle_t>().each()) {
        auto [rx, ry]   = render_position(pos, camera);
        const auto& vch = rend.vchar;
        rltk::xchar xch = {
            vch.glyph,
            vch.foreground,
            static_cast<float>(rx),
            static_cast<float>(ry),
        };
        rltk::sterm(gui::UI_PARTICLES)->add(xch);
    }
}

}  // namespace radl::system
