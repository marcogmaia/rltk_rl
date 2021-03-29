#include "component/renderable.hpp"
#include "component/position.hpp"
#include "component/viewshed.hpp"
#include "rltk/rltk.hpp"
#include "map.hpp"
#include "spdlog/spdlog.h"
#include "camera.hpp"


namespace radl {

void camera_update(entt::registry& reg, entt::entity ent) {
    using rltk::console;
    console->clear();
    using namespace world;
    const auto& [pos, rend, vshed]
        = reg.get<position_t, Renderable, viewshed_t>(ent);
    const auto& [px, py] = pos;

    auto ent_map = reg.view<world::Map>().front();
    if(!reg.valid(ent_map)) {
        spdlog::error("entity with map ent_map, doesn't exist");
        return;
    }
    auto& map = reg.get<world::Map>(ent_map);

    update_viewshed(pos, vshed, map);

    auto xi = px - console->term_width / 2;
    auto xf = px + console->term_width / 2;
    auto yi = py - console->term_height / 2;
    auto yf = py + console->term_height / 2;

    // render explored map
    for(int x = xi; x < xf; ++x) {
        for(int y = yi; y < yf; ++y) {
            if(!map.rect.contains({x, y})) {
                continue;
            }
            auto& tile = map[{x, y}];
            // TODO implement explored function
            // TODO print player
            if(tile.is_explored) {
                switch(tile.type) {
                case tile_type_t::floor: {
                    tile.vchar.glyph = '.';
                } break;
                case tile_type_t::wall: {
                    tile.vchar.glyph = '#';
                } break;
                default:
                    break;
                }
                tile.vchar.foreground = rltk::colors::DARKEST_GREY;
            }
            else {
                tile.vchar = {'.', BLACK, BLACK};
            }
            // TODO if in FoV
            console->set_char(x - xi, y - yi, tile.vchar);
        }
    }
    // render visible tiles
    for(auto& pos : vshed.visible_coordinates) {
        const auto& [x, y] = pos;
        if(!map.rect.contains({x - xi, y - yi})) {
            continue;
        }
        auto vis_vchar       = map[pos].vchar;
        vis_vchar.foreground = WHITE;
        console->set_char(x - xi, y - yi, vis_vchar);
    }
    // render player
    console->set_char(px - xi, py - yi, rend.vchar);
}

}  // namespace radl
