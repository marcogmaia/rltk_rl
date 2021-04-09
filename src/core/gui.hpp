#pragma once
#include "rltk/rltk.hpp"

#include "entt/entt.hpp"

namespace radl {

using rltk::term;

}

namespace radl::gui {

constexpr int GUI_MAP          = 0;
constexpr int GUI_ENTITIES     = 1;
constexpr int GUI_STATUS       = 2;
constexpr int GUI_LOG          = 3;
constexpr int UI_PLAYER_HEALTH = 4;

void init();
void render_gui();

}  // namespace radl::gui
