#include <algorithm>
#include <map>

#include "spdlog/spdlog.h"

#include "core/gui/gui.hpp"
#include "core/gui/item_menu.hpp"
#include "core/engine.hpp"

#include "component/component.hpp"

#include "utils/utils.hpp"


#include "imgui.h"
#include "imgui-SFML.h"


namespace radl::gui {

namespace {

using namespace rltk::colors;
using engine::player;
using engine::reg;
using rltk::gui;


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
        auto *name = reg.try_get<name_t>(ent);
        if(name != nullptr) {
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

void resize_main(rltk::layer_t* l, int w, int h) {
    // Simply set the width to the whole window width
    l->w = w;
    l->h = h;
}

void resize_inventory_popup(rltk::layer_t* l, int w, int h) {
    // Simply set the width to the whole window width
    l->w = l->w;
    l->h = l->h;
    l->x = 32;
    l->y = 32;
}

void resize_status(rltk::layer_t* l, int w, int h) {
    auto [fw, fh] = term(UI_STATUS)->get_font_size();

    l->w = (w / fw) * fw - (20 + 3) * fw;
    l->h = (2 + 8) * fh;
    l->y = ((h / fh) - (l->h / fh)) * fh;
}

void resize_inventory(rltk::layer_t* l, int w, int h) {
    auto [fw, fh] = term(UI_INVENTORY)->get_font_size();

    l->w = (20 + 2) * fw;
    l->h = (h / fh) * fh;
    l->x = ((w / fw) - (l->w / fw)) * fw;
    l->y = (h / fh - l->h / fh) * fh;
}

#ifdef DEBUG

void imgui_process_event(const sf::Event& event) {
    if(event.type == sf::Event::EventType::KeyPressed
       || event.type == sf::Event::EventType::KeyReleased) {
        if(event.key.code < 0) {
            return;
        }
    }
    ImGui::SFML::ProcessEvent(event);
}

void imgui_frame(rltk::layer_t* l, sf::RenderTexture& window) {
    static sf::Clock deltaClock;
    auto& main_window = *rltk::get_window();
    ImGui::SFML::Update(main_window, deltaClock.restart());

    ImGui::Begin("Add Items");
    auto pressed = ImGui::Button("+ healing potion.");
    if(pressed) {
        reg.get<inventory_t>(player).add_item(items::potion_healing(true));
    }
    ImGui::End();

    ImGui::Begin("test wind");
    ImGui::End();

    // ImGui::EndFrame();
    ImGui::SFML::Render(main_window);
}

void imgui_init() {
    ImGui::SFML::Init(*rltk::get_window());
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    engine::event_dispatcher.sink<sf::Event>().connect<imgui_process_event>();
    gui->add_owner_layer(GOD_UI, 0, 0, 1024, 768, resize_main, imgui_frame);
}

#endif

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
    gui->add_sparse_layer(UI_PARTICLES, map_rect.x1, map_rect.y1, map_rect.x2,
                          map_rect.y2, "16x16", resize_main);

    gui->add_layer(UI_STATUS, status_rect.x1, status_rect.y1, status_rect.x2,
                   status_rect.y2, "8x16", resize_status, true);
    gui->add_layer(UI_INVENTORY, invent_rect.x1, invent_rect.y1, invent_rect.x2,
                   invent_rect.y2, "8x16", resize_inventory, false);
    gui->add_layer(UI_INVENTORY_POPUP, invent_rect.x1, invent_rect.y1,
                   invent_rect.x2, invent_rect.y2, "8x16",
                   resize_inventory_popup, false);

    // cursor
    gui->add_layer(UI_MOUSE, map_rect.x1, map_rect.y1, map_rect.x2, map_rect.y2,
                   "16x16", resize_main, true);
    // tooltips
    gui->add_layer(UI_TOOLTIPS, map_rect.x1, map_rect.y1, map_rect.x2,
                   map_rect.y2, "16x16", resize_main, true);
    term(UI_MOUSE)->set_alpha(127);
    term(UI_TOOLTIPS)->set_alpha(0xDF);

    engine::console = term(UI_MAP);

#ifdef DEBUG
    imgui_init();
#endif
}

void terminate() {
#ifdef DEBUG
    ImGui::SFML::Shutdown();
#endif
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


void render_log_stats() {
    term(UI_STATUS)->clear();
    term(UI_STATUS)->box(GREY, BLACK, true);
    // print game log
    auto& log = engine::get_game_log().entries;
    int i     = 1;
    std::for_each_n(log.crbegin(), std::min(static_cast<int>(log.size()), 8),
                    [&i](const component::log_entry_t& entry) {
                        term(UI_STATUS)->print(1, i++, entry.log, entry.fg,
                                               entry.bg);
                    });

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
}

void render_inventory_items() {
    auto* inventory = reg.try_get<inventory_t>(player);
    if(inventory == nullptr) {
        return;
    }
    int row = 0;
    for(auto [item_id, item_bucket] : inventory->items) {
        if(item_bucket.empty()) {
            continue;
        }
        // for(auto& ent_item : item_bucket) {
        auto first_item = item_bucket.back();
        auto& item_name = reg.get<name_t>(first_item).name;
        auto item_str   = fmt::format("{}x {}", item_bucket.size(), item_name);
        term(UI_INVENTORY)->print(2, 1 + row, item_str);
        ++row;
    }
}

void render_inventory_right_side() {
    term(UI_INVENTORY)->clear();
    term(UI_INVENTORY)->box(GREY, BLACK, true);
    term(UI_INVENTORY)->print_center(0, "Inventory");
    render_inventory_items();
}

void render_gui() {
    render_inventory_right_side();
    render_log_stats();
    render_mouse_overlay();
}

void clear_gui() {
    term(UI_INVENTORY)->clear();
    term(UI_INVENTORY_POPUP)->clear();
}

}  // namespace radl::gui
