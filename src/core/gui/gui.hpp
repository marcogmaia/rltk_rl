#pragma once
#include "rltk/rltk.hpp"

#include "entt/entt.hpp"
#include "core/gui/item_menu.hpp"

namespace radl {

using rltk::term;

}

namespace radl::gui {


enum ui_order_t {
UI_MAP,
UI_ENTITIES,
UI_STATUS,
UI_INVENTORY,
UI_INVENTORY_POPUP,
UI_LOG,
UI_PLAYER_HEALTH,
UI_MOUSE,
UI_TOOLTIPS,
};

void init();
void render_gui();
void clear_gui();

// void render_inventory(std::function<char(void)> get_option);

}  // namespace radl::gui
