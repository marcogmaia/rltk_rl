#include <ranges>
#include <algorithm>
#include <execution>

#include "spdlog/spdlog.h"
#include "rltk/rltk.hpp"

#include "core/map.hpp"
#include "core/gui.hpp"
#include "system/camera.hpp"
#include "component/component.hpp"


namespace radl {


void camera_update(entt::entity ent) {
    using namespace world;
    using engine::reg;
    // using rltk::console;
    using engine::console;
    auto& map = engine::get_map();

    // console->clear();
    term(gui::GUI_MAP)->clear();
    term(gui::GUI_ENTITIES)->clear();

    const auto& [player_pos, rend, pvshed]
        = reg.get<position_t, renderable_t, viewshed_t>(ent);
    const auto& [px, py] = player_pos;

    auto xci = px - console->term_width / 2;
    auto xcf = px + console->term_width / 2;
    auto yci = py - console->term_height / 2;
    auto ycf = py + console->term_height / 2;

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
            const auto& tile = map[{x, y}];
            if(!tile.characteristics.explored) {
                continue;
            }
            auto vch       = tile.get_vchar();
            vch.foreground = color_t(15,15,15);
            // vch.foreground = DARKEST_GREY;
            auto renderpos = render_pos(x, y);
            console->set_char(renderpos.first, renderpos.second, vch);
        }
    }

    // render visible tiles
    for(const auto& v_pos : pvshed.visible_coordinates) {
        const auto& tile = map.at(v_pos);
        vchar_t tile_vch = tile.get_vchar();
        if(tile.type == tile_type_t::wall) {
            tile_vch.foreground = color_t(0, 31, 36);
        }
        else if(tile.type == tile_type_t::floor) {
            if(tile.status == tile_status_t::BLOODIED) {
                tile_vch.foreground = DARKER_RED;
            }
            else {
                tile_vch.foreground = color_t(36, 18, 4);
            }
        }
        auto [rx, ry] = render_pos(v_pos.first, v_pos.second);
        term(gui::GUI_MAP)->set_char(rx, ry, tile_vch);
    }

    // search entities in visible positions and print them
    std::for_each(
        std::begin(pvshed.visible_coordinates),
        std::end(pvshed.visible_coordinates), [&](const position_t& vpos) {
            const auto& ents = map[vpos].entities_here;
            for(const auto& ent : ents) {
                if(!reg.all_of<renderable_t, position_t>(ent)) {
                    continue;
                }
                const auto& e_rend   = reg.get<renderable_t>(ent);
                const auto& e_pos    = reg.get<position_t>(ent);
                const auto& [rx, ry] = render_pos(e_pos.first, e_pos.second);
                term(gui::GUI_ENTITIES)->set_char(rx, ry, e_rend.vchar);
            }
        });

    // render player
    auto player_vch = rend.vchar;
    term(gui::GUI_ENTITIES)->set_char(px - xci, py - yci, player_vch);
}

}  // namespace radl
