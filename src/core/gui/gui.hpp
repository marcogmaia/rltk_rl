#pragma once
#include "rltk/rltk.hpp"

#include "entt/entt.hpp"
#include "core/gui/item_menu.hpp"

namespace radl {

using rltk::term;

}

namespace radl::gui {


void init();
void terminate();

void render_gui(double elapsed_time_ms);
void clear_gui();

// void render_inventory_use(std::function<char(void)> get_option);

}  // namespace radl::gui
