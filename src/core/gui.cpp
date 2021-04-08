#include "core/gui.hpp"
#include "core/engine.hpp"
#include "component/combat.hpp"


namespace radl::gui {


namespace {

using namespace rltk::colors;
using engine::player;
using engine::reg;
using rltk::gui;
using rltk::term;


constexpr int GUI_MAP      = 0;
constexpr int GUI_ENTITIES = 1;
constexpr int GUI_STATUS   = 2;
constexpr int GUI_LOG      = 3;

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

    auto hp_fmt = fmt::format("HP:{:3d}/{:3d} ", pstats.hp, pstats.max_hp);
    int xhp     = 2;
    int yhp     = 2;
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
                   resize_box, false);

    engine::console = term(GUI_ENTITIES);
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


    term(GUI_STATUS)->print(3, 0, "stats");
}

}  // namespace radl::gui
