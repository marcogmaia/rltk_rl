#pragma once
#include <tuple>
#include "entt/entity/registry.hpp"
#include "rltk/rltk.hpp"

namespace radl::gui {

enum ui_order_t {
    UI_MAP,
    UI_ENTITIES,
    UI_PARTICLES,
    UI_STATUS,
    UI_INVENTORY,
    UI_INVENTORY_POPUP,
    UI_LOG,
    UI_PLAYER_HEALTH,
    UI_MOUSE,
    UI_TOOLTIPS,
    GOD_UI,
};


enum item_menu_result_t {
    CANCEL,
    NO_RESPONSE,
    SELECTED,
};

std::tuple<item_menu_result_t, entt::entity> render_inventory_use();

std::tuple<item_menu_result_t, entt::entity> render_inventory_drop();

}  // namespace radl::gui
