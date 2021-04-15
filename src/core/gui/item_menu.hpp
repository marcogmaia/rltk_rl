#pragma once
#include <tuple>
#include "entt/entity/registry.hpp"

namespace radl::gui {

enum item_menu_result_t {
    CANCEL,
    NO_RESPONSE,
    SELECTED,
};

std::tuple<item_menu_result_t, entt::entity> render_inventory();

}  // namespace radl::gui
