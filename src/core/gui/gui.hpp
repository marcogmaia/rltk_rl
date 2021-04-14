#pragma once
#include "rltk/rltk.hpp"

#include "entt/entt.hpp"

namespace radl {

using rltk::term;

}

namespace radl::gui {


enum ui_order_t {
UI_MAP,
UI_ENTITIES,
UI_STATUS,
UI_INVENTORY,
UI_LOG,
UI_PLAYER_HEALTH,
UI_MOUSE,
UI_TOOLTIPS,
};

void init();
void render_gui();


void show_inventory();

}  // namespace radl::gui
