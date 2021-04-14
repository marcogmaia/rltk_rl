#include <algorithm>
#include <map>

#include "spdlog/spdlog.h"

#include "core/gui/gui.hpp"
#include "core/engine.hpp"

#include "component/component.hpp"


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


void render_hp_bar(int x, int y, vchar_t fg_vch, vchar_t bg_vch) {
    const auto& pstats = reg.get<component::combat_stats_t>(player);

    // print HP bar
    auto hp_fmt = fmt::format("HP:{:3d}/{:3d} ", pstats.hp, pstats.max_hp);
    int xhp     = 2;
    int yhp     = 0;
    term(UI_STATUS)->print(xhp, yhp, hp_fmt, YELLOW);

    double divs   = 50.0;
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

void draw_tooltips() {
    term(UI_TOOLTIPS)->clear();

    auto& map = engine::get_map();
    // auto view_tooltip = reg.view<name_t, position_t>();

    auto mouse_pos = state::get_mouse_position();
    auto [mx, my]  = mouse_pos;
    auto [fw, fh]  = term(UI_MOUSE)->get_font_size();
    mx /= fw;
    my /= fh;

    auto playerpos = reg.get<position_t>(player);
    auto& pvshed   = reg.get<viewshed_t>(player);

    auto console_pos = position_t{engine::console->term_width / 2,
                                  engine::console->term_height / 2};

    auto mpos_relative_player = playerpos + position_t{mx, my} - console_pos;

    if(!map.rect.contains(mpos_relative_player)
       || !pvshed.visible_coordinates.contains(mpos_relative_player)) {
        return;
    }

    std::vector<std::string> visible_names;
    for(const auto& ent : map.at(mpos_relative_player).entities_here) {
        auto name = reg.try_get<name_t>(ent);
        if(name) {
            visible_names.push_back(name->name);
            spdlog::debug("range: {}", name->name);
        }
    }


    if(!visible_names.empty()) {
        auto max_height = visible_names.size();
        auto max_elem
            = std::max_element(visible_names.begin(), visible_names.end(),
                               [](std::string& lhs, std::string& rhs) {
                                   return lhs.size() < rhs.size();
                               });
        auto box_width   = max_elem->size() + 4;
        auto box_height  = max_height + 1;
        color_t bg_color = DARKEST_GREEN;
        term(UI_TOOLTIPS)->resize_chars(box_width, box_height);
        term(UI_TOOLTIPS)
            ->fill(0, 0, box_width, box_height, ' ', WHITE, bg_color);
        term(UI_TOOLTIPS)
            ->set_offset((mouse_pos.first + fw * 1) / fw * fw,
                         (mouse_pos.second / fh) * fh);
        // term(UI_TOOLTIPS)->box(WHITE, BLACK, true);
        int row = 0;
        for(auto& name : visible_names) {
            term(UI_TOOLTIPS)
                ->set_char(0, 1 + row, vchar_t{glyph::LIST_MID, WHITE, BLACK});
            term(UI_TOOLTIPS)
                ->set_char(1, 1 + row,
                           vchar_t{glyph::HORIZONTAL_LINE, WHITE, BLACK});
            term(UI_TOOLTIPS)->print(2, 1 + row, name, WHITE, bg_color);
            ++row;
        }
    }
}

void resize_status(rltk::layer_t* l, int w, int h) {
    auto [fw, fh] = term(UI_STATUS)->get_font_size();

    l->w = (w / fw) * fw - (16 + 3) * fw;
    l->h = (2 + 8) * fh;
    l->y = ((h / fh) - (l->h / fh)) * fh;
}

void resize_inventory(rltk::layer_t* l, int w, int h) {
    auto [fw, fh] = term(UI_INVENTORY)->get_font_size();

    l->w = (16 + 2) * fw;
    l->h = (h / fh) * fh;
    l->x = ((w / fw) - (l->w / fw)) * fw;
    l->y = (h / fh - l->h / fh) * fh;
}

void init() {
    rect_t invent_rect{
        1024 - 16 * 16,
        0,
        1024,
        768,
    };
    rect_t map_rect = {
        0,
        0,
        1024,
        768 - 16 * 10,
    };
    rect_t status_rect = {
        1 * 12,
        map_rect.y2,
        12 + 144,
        768,
    };

    gui->add_layer(UI_MAP, map_rect.x1, map_rect.y1, map_rect.x2, map_rect.y2,
                   "16x16", resize_main, true);
    gui->add_layer(UI_ENTITIES, map_rect.x1, map_rect.y1, map_rect.x2,
                   map_rect.y2, "16x16", resize_main, false);

    gui->add_layer(UI_STATUS, status_rect.x1, status_rect.y1, status_rect.x2,
                   status_rect.y2, "8x16", resize_status, true);
    gui->add_layer(UI_INVENTORY, invent_rect.x1, invent_rect.y1, invent_rect.x2,
                   invent_rect.y2, "8x16", resize_inventory, false);

    gui->add_layer(UI_MOUSE, map_rect.x1, map_rect.y1, map_rect.x2, map_rect.y2,
                   "16x16", resize_main, true);
    gui->add_layer(UI_TOOLTIPS, map_rect.x1, map_rect.y1, map_rect.x2,
                   map_rect.y2, "8x16", resize_main, true);
    term(UI_MOUSE)->set_alpha(127);
    term(UI_TOOLTIPS)->set_alpha(0xDF);

    engine::console = term(UI_MAP);
}

void render_mouse_overlay() {
    auto [mx, my] = state::get_mouse_position();
    auto [fw, fh] = term(UI_MOUSE)->get_font_size();
    mx /= fw;
    my /= fh;
    term(UI_MOUSE)->clear();
    term(UI_MOUSE)->set_char(mx, my, vchar_t{glyph::SOLID1, ORANGE, BLACK});
    draw_tooltips();
}

// TODO show inventory when I is pressed
void show_inventory() {
    // ## O(n*log(n))
    auto& inventory = reg.get<inventory_t>(player);
    auto& items     = inventory.items;
    auto iname      = get_name(items.front());

    std::multimap<std::string, entt::entity> mumap;
    std::ranges::transform(items, std::inserter(mumap, mumap.begin()),
                           [](entity ent) {
                               auto iname = get_name(ent);
                               return std::make_pair(iname, ent);
                           });

    std::set<std::string> keys;
    for(auto& entry : mumap) {
        keys.insert(entry.first);
    }
    for(auto& key : keys) {
        spdlog::debug("k: {} num: {}", key, mumap.count(key));
    }
}

void render_inventory_items() {
    auto* inventory = reg.try_get<inventory_t>(player);
    if(!inventory) {
        return;
    }

    int row = 0;
    for(auto& item : inventory->items) {
        auto& item_name = reg.get<name_t>(item).name;
        term(UI_INVENTORY)->print(2, 1 + row, item_name);
        ++row;
    }
}

void render_gui() {
    term(UI_STATUS)->clear();

    term(UI_STATUS)->box(GREY, BLACK, true);

    term(UI_INVENTORY)->clear();
    term(UI_INVENTORY)->box(GREY, BLACK, true);
    term(UI_INVENTORY)->print_center(0, "Inventory");
    render_inventory_items();

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
