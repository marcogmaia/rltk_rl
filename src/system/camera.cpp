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

// template <typename... Component, typename... Exclude, typename Entity>
inline void
render_particles(const camera_t& camera,
                 entt::basic_view<entt::entity, entt::exclude_t<>, renderable_t,
                                  position_t, particle_t>&& view) {
    rltk::sterm(gui::UI_PARTICLES)->clear();
    for(auto ent : view) {
        auto [rend, pos, part] = view.get(ent);
        auto [rx, ry]          = render_position(pos, camera);
        const auto& vch        = rend.vchar;
        rltk::xchar xch{
            vch.glyph,
            vch.foreground,
            static_cast<float>(rx),
            static_cast<float>(ry),
        };
        rltk::sterm(gui::UI_PARTICLES)->add(xch);
    }
}

// template <typename IterView>
inline void
render_renderables(const camera_t& camera,
                   entt::basic_view<entt::entity, entt::exclude_t<>,
                                    renderable_t, position_t>&& view) {
    // TODO only needs to sort when a new renderable is added or removed
    reg.sort<renderable_t>(
        [](const renderable_t& lhs, const renderable_t& rhs) {
            return lhs.z_level < rhs.z_level;
        });
    view.use<renderable_t>();
    auto& pvshed = reg.get<viewshed_t>(player);
    for(auto ent : view) {
        auto [rend, rpos] = view.get(ent);
        if(!camera.reveal_map) {
            if(!pvshed.visible_coordinates.contains(rpos)) {
                continue;
            }
        }
        auto [rx, ry] = render_position(rpos, camera);
        term(gui::UI_ENTITIES)->set_char(rx, ry, rend.vchar);
    }
}


inline void render_explored_map(const camera_t& camera) {
    auto& base_map = reg.ctx<Map>();

    auto& [cx, cy] = camera.position;
    auto xci       = cx - camera.width / 2;
    auto yci       = cy - camera.height / 2;
    auto xcf       = cx + camera.width / 2;
    auto ycf       = cy + camera.height / 2;

    for(int x = xci; x < xcf; ++x) {
        for(int y = yci; y < ycf; ++y) {
            // if(!should_update({x,y})) continue;
            if(!base_map.rect.contains({x, y})) {
                continue;
            }
            const auto& tile = base_map.at({x, y});

            if(camera.reveal_map) {
            } else if(!tile.props.explored) {
                continue;
            }
            auto vch       = tile.get_vchar();
            vch.foreground = color_t(15, 15, 15);
            // vch.foreground = DARKEST_GREY;
            auto [rx, ry] = render_position(position_t{x, y}, camera);
            term(gui::UI_MAP)->set_char(rx, ry, vch);
        }
    }
}

inline void render_visible_map(const camera_t& camera) {
    auto& base_map = reg.ctx<Map>();
    auto& pvshed   = reg.get<viewshed_t>(player);

    if(camera.reveal_map) {
        for(int x = 0; x < base_map.rect.width(); ++x) {
            for(int y = 0; y < base_map.rect.height(); ++y) {
                auto& tile    = base_map.at(x, y);
                auto tile_vch = tile.get_vchar();
                auto [rx, ry] = render_position(position_t{x, y}, camera);
                if(tile.type == tile_type_t::wall) {
                    tile_vch.foreground = color_t(0, 31, 36);
                } else if(tile.type == tile_type_t::floor) {
                    if(tile.status == tile_status_t::BLOODIED) {
                        tile_vch.foreground = DARKER_RED;
                    } else {
                        tile_vch.foreground = color_t(36, 18, 4);
                    }
                }
                term(gui::UI_MAP)->set_char(rx, ry, tile_vch);
            }
        }
    } else {
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
            }
            term(gui::UI_MAP)->set_char(rx, ry, tile_vch);
        }
    }
    for(const auto& v_pos : pvshed.visible_coordinates) {
        const auto& tile = base_map.at(v_pos);
        if(tile.type == tile_type_t::floor) {
            auto [rx, ry] = render_position(v_pos, camera);
            term(gui::UI_ENTITIES)
                ->set_char(rx, ry, vchar_t{glyph::CDOT, DARK_GREY, BLACK});
        }
    }
}

// TODO render_dijkstra_map
void render_dijkstra_map() {
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

void render_camera(camera_t& camera, [[maybe_unused]] bool debug = false) {
    if(!camera.dirty) {
        return;
    }
    camera.dirty = false;
    term(gui::UI_MAP)->clear();
    term(gui::UI_ENTITIES)->clear();
    render_explored_map(camera);
    render_visible_map(camera);
    render_renderables(camera, reg.view<renderable_t, position_t>());
    render_particles(camera, reg.view<renderable_t, position_t, particle_t>());
}

void camera() {
    auto& camera = reg.ctx<camera_t>();
    if(camera.reveal_map) {
        render_camera(camera, true);
    } else {
        render_camera(camera, false);
    }
}

}  // namespace radl::system
