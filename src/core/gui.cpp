#include <algorithm>

#include "core/gui.hpp"
#include "core/engine.hpp"
#include "component/combat.hpp"
#include "component/log.hpp"


namespace radl::gui {

namespace {

using namespace rltk::colors;
using engine::player;
using engine::reg;
using rltk::gui;


void resize_main(rltk::layer_t* l, int w, int h) {
    // Simply set the width to the whole window width
    l->w = w;
    l->h = h;
}

void resize_box(rltk::layer_t* l, int w, int h) {
    auto [fw, fh] = term(UI_STATUS)->get_font_size();

    l->w = w - 2 * fw;
    l->h = (2 + 8) * fh;
    l->y = h - l->h;
}

void render_hp_bar(int x, int y, vchar_t fg_vch, vchar_t bg_vch) {
    const auto& pstats = reg.get<combat_stats_t>(player);

    // print HP bar
    auto hp_fmt = fmt::format("HP:{:3d}/{:3d} ", pstats.hp, pstats.max_hp);
    int xhp     = 2;
    int yhp     = 0;
    term(UI_STATUS)->print(xhp, yhp, hp_fmt, YELLOW);

    double divs   = 100.0;
    auto hp_index = static_cast<int>(std::ceil(pstats.hp / (100.0 / divs)));
    auto hp_index_max
        = static_cast<int>(std::ceil(pstats.max_hp / (100.0 / divs)));

    for(int i = 0; i < hp_index_max; ++i) {
        term(UI_STATUS)->set_char(xhp + i + hp_fmt.size(), yhp, bg_vch);
    }
    for(int i = 0; i < hp_index; ++i) {
        term(UI_STATUS)->set_char(xhp + i + hp_fmt.size(), yhp, fg_vch);
    }
}

}  // namespace

// TODO add mouse hover tooltip


void init() {
    rect_t mrect = {
        0,
        0,
        1024,
        768 - 16 * 10,
    };
    rect_t srect = {
        1 * 12,
        mrect.y2,
        12 + 144,
        768,
    };

    gui->add_layer(UI_MAP, mrect.x1, mrect.y1, mrect.x2, mrect.y2, "16x16",
                   resize_main, true);
    gui->add_layer(UI_ENTITIES, mrect.x1, mrect.y1, mrect.x2, mrect.y2, "16x16",
                   resize_main, false);

    gui->add_layer(UI_STATUS, srect.x1, srect.y1, srect.x2, srect.y2, "8x16",
                   resize_box, true);

    gui->add_layer(UI_MOUSE, mrect.x1, mrect.y1, mrect.x2, mrect.y2, "16x16",
                   resize_main, false);
    term(UI_MOUSE)->set_alpha(127);

    // gui->add_layer(UI_INVENTORY);

    engine::console = term(UI_MAP);
}

void render_mouse_overlay() {
    auto [mx, my] = state::get_mouse_position();
    auto [fw, fh] = term(UI_MOUSE)->get_font_size();
    mx /= fw;
    my /= fh;
    term(UI_MOUSE)->clear();
    term(UI_MOUSE)->set_char(mx, my, vchar_t{glyph::SOLID1, ORANGE, BLACK});
}

void render_gui() {
    term(UI_STATUS)->clear();

    term(UI_STATUS)->box(GREY, BLACK, true);

    render_hp_bar(1, 3,
                  vchar_t{
                      glyph::BLOCK2,
                      RED,
                      BLACK,
                  },
                  vchar_t{
                      glyph::BLOCK2,
                      DARKEST_RED,
                      BLACK,
                  });

    // print game log
    auto& log = engine::get_game_log().entries;
    int i     = 1;
    std::for_each_n(log.crbegin(), std::min(static_cast<int>(log.size()), 8),
                    [&i](const component::log_entry_t& entry) {
                        term(UI_STATUS)->print(1, i++, entry.log, entry.fg,
                                               entry.bg);
                    });

    render_mouse_overlay();
}

}  // namespace radl::gui
