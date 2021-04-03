#include <ranges>
#include <algorithm>
#include <execution>

#include "spdlog/spdlog.h"
#include "rltk/rltk.hpp"

#include "core/map.hpp"
#include "system/camera.hpp"
#include "component/component.hpp"


namespace radl {


void camera_update(entt::registry& reg, entt::entity ent) {
    using namespace world;
    using rltk::console;
    auto& map = reg.ctx<Map>();

    console->clear();
    const auto& [player_pos, rend, viewshed]
        = reg.get<position_t, renderable_t, viewshed_t>(ent);
    const auto& [px, py] = player_pos;

    auto xci = px - console->term_width / 2;
    auto xcf = px + console->term_width / 2;
    auto yci = py - console->term_height / 2;
    auto ycf = py + console->term_height / 2;

    // rect_t render_viewport = {
    //     0,
    //     0,
    //     console->term_width,
    //     console->term_height,
    // };

    auto render_pos = [&](int xr, int yr) -> position_t {
        return position_t{xr - xci, yr - yci};
    };

    // render all explored
    for(int x = xci; x < xcf; ++x) {
        for(int y = yci; y < ycf; ++y) {
            // if(!should_update({x,y})) continue;
            if(!map.rect.contains({x, y})) {
                continue;
            }
            auto tile = map[{x, y}];
            if(!tile.characteristics.explored) {
                continue;
            }
            auto vch       = tile.get_vchar();
            vch.foreground = DARKEST_GREY;
            auto renderpos = render_pos(x, y);
            console->set_char(renderpos.first, renderpos.second, vch);
        }
    }


    // render visible tiles and enemies
    // auto& ents_near = *get_entities_near_player();
    auto pvshed     = reg.get<viewshed_t>(engine::player);
    auto g_enemy    = reg.group<enemy_t, renderable_t, position_t>();
    for(auto& v_pos : pvshed.visible_coordinates) {
        const auto& tile = map.at(v_pos);
        vchar_t tile_vch = tile.get_vchar();
        // tile_vch.background = BLACK;
        if(tile.type == tile_type_t::wall) {
            // tile_vch.glyph      = glyph::SOLID1;
            tile_vch.foreground = LimeGreen;
        }
        else if(tile.type == tile_type_t::floor) {
            // tile_vch.glyph      = glyph::BLOCK1;
            tile_vch.foreground = SkyBlue;
        }
        auto [rx, ry] = render_pos(v_pos.first, v_pos.second);
        console->set_char(rx, ry, tile_vch);
    }

    // parallel check and render enemy
    auto& vcoords                = pvshed.visible_coordinates;
    auto render_enemy_if_visible = [&](position_t& v_pos) {
        g_enemy.each([&](auto ent, renderable_t& e_rend, position_t& e_pos) {
            if(v_pos == e_pos) {
                auto [rx, ry] = render_pos(e_pos.first, e_pos.second);
                console->set_char(rx, ry, e_rend.vchar);
            }
        });
    };
    std::for_each(std::execution::par_unseq, vcoords.begin(), vcoords.end(),
                  render_enemy_if_visible);

    auto player_vch = rend.vchar;
    console->set_char(px - xci, py - yci, player_vch);
}

}  // namespace radl
