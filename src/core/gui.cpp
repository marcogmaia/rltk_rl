#include <algorithm>

#include "core/gui.hpp"
#include "core/engine.hpp"
#include "component/combat.hpp"


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
    auto [fw, fh] = term(GUI_STATUS)->get_font_size();

    l->w = w - 2 * fw;
    l->h = (2 + 8) * fh;
    l->y = h - l->h;
}

void print_hp_bar(int x, int y, vchar_t fg_vch, vchar_t bg_vch) {
    const auto& pstats = reg.get<combat_stats_t>(player);

    // print HP bar
    auto hp_fmt = fmt::format("HP:{:3d}/{:3d} ", pstats.hp, pstats.max_hp);
    int xhp     = 2;
    int yhp     = 0;
    term(GUI_STATUS)->print(xhp, yhp, hp_fmt, YELLOW);

    double divs   = 100.0;
    auto hp_index = static_cast<int>(std::ceil(pstats.hp / (100.0 / divs)));
    auto hp_index_max
        = static_cast<int>(std::ceil(pstats.max_hp / (100.0 / divs)));

    for(int i = 0; i < hp_index_max; ++i) {
        term(GUI_STATUS)->set_char(xhp + i + hp_fmt.size(), yhp, bg_vch);
    }
    for(int i = 0; i < hp_index; ++i) {
        term(GUI_STATUS)->set_char(xhp + i + hp_fmt.size(), yhp, fg_vch);
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

    gui->add_layer(GUI_MAP, mrect.x1, mrect.y1, mrect.x2, mrect.y2, "16x16",
                   resize_main, true);
    gui->add_layer(GUI_ENTITIES, mrect.x1, mrect.y1, mrect.x2, mrect.y2,
                   "16x16", resize_main, false);

    gui->add_layer(GUI_STATUS, srect.x1, srect.y1, srect.x2, srect.y2, "8x16",
                   resize_box, true);

    // rltk::term(GUI_STATUS)->set_alpha(255);

    // gui->get_layer(GUI_STATUS)
    //     ->add_hbar(UI_PLAYER_HEALTH, 1, 1, 100, 0, 100, 100, RED, RED,
    //                DARKEST_RED, DARKEST_RED, YELLOW, "Health: ");

    engine::console = term(GUI_MAP);
}

void render_gui() {
    [[maybe_unused]] auto [fw, fh] = term(GUI_STATUS)->get_font_size();
    term(GUI_STATUS)->clear();

    term(GUI_STATUS)->box(GREY, BLACK, true);

    print_hp_bar(1, 3,
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
                    [&i](const std::string& entry) {
                        term(GUI_STATUS)->print(1, i++, entry);
                    });
}

}  // namespace radl::gui
