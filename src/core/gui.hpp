#pragma once
#include "rltk/rltk.hpp"

#include "entt/entt.hpp"

namespace radl {

using rltk::term;

}

namespace radl::gui {

constexpr int UI_MAP           = 0;
constexpr int UI_ENTITIES      = 1;
constexpr int UI_STATUS        = 2;
constexpr int UI_LOG           = 3;
constexpr int UI_PLAYER_HEALTH = 4;
constexpr int UI_MOUSE         = 5;

void init();
void render_gui();

}  // namespace radl::gui
