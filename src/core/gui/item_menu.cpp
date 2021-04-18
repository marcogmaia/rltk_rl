#include "component/component.hpp"

#include "core/engine.hpp"
#include "core/gui/gui.hpp"
#include "core/gui/item_menu.hpp"

#include "utils/utils.hpp"


namespace radl::gui {

int ev_to_option(sf::Event ev) {
    return ev.key.code;
}

int get_option() {
    auto& evq = engine::event_queue;
    if(evq.empty()) {
        return -1;
    }
    auto ev = evq.front();
    evq.pop_front();
    auto opt = ev_to_option(ev);

    return opt;
}

void render_inventory(const inventory_t& inventory, const char* inv_name) {
    auto& inv_ui = *term(UI_INVENTORY_POPUP);
    inv_ui.clear();
    const auto& gstate = reg.ctx<engine::game_state_t>();
    if(!(gstate == engine::game_state_t::SHOW_INVENTORY
         || gstate == engine::game_state_t::SHOW_INVENTORY_DROP)) {
        return;
    }

    // resize inventory UI
    if(inventory.items.empty()) {
        inv_ui.resize_chars(18 + 2, 3);
    } else {
        auto longest_string
            = *longest_string_in_collection(inventory.get_unique_item_names());
        inv_ui.resize_chars(std::max(size_t(18), longest_string.size() + 4) + 5,
                            inventory.items.size() + 3);
    }
    // offset in pixels
    inv_ui.set_offset(32, 32);
    inv_ui.box();
    inv_ui.print_center(0, inv_name);
    // print unique items and their number
    std::vector<std::string> vec_keys;
    int i = 0;
    for(const auto& [item_id, item_bucket] : inventory.items) {
        auto first_item = item_bucket.back();
        auto item_name  = reg.get<name_t>(first_item).name;
        auto item_str = fmt::format("{:c}) {}x-{}", 'a' + i, item_bucket.size(),
                                    item_name);
        inv_ui.print(1, 1 + i, item_str);
        auto opt_str = item_str.substr(0, 2);
        inv_ui.print(1, 1 + i, opt_str, YELLOW);
        ++i;
    }
    inv_ui.print(2, 2 + i, "ESC-Cancel", YELLOW);
}

std::tuple<item_menu_result_t, entity> render_inventory_use() {
    auto& inventory = reg.get<inventory_t>(player);
    // get_option
    entity ent_ret              = entt::null;
    item_menu_result_t menu_res = item_menu_result_t::NO_RESPONSE;
    auto opt                    = static_cast<size_t>(get_option());

    render_inventory(inventory, "inventory");

    if(opt < inventory.items.size()) {
        menu_res = item_menu_result_t::SELECTED;
        auto it  = std::next(inventory.items.begin(), opt);
        ent_ret  = it->second.back();
    } else if(opt == sf::Keyboard::Escape) {
        menu_res = item_menu_result_t::CANCEL;
    }

    return std::make_tuple(menu_res, ent_ret);
}

std::tuple<item_menu_result_t, entity> render_inventory_drop() {
    auto& inventory             = reg.get<inventory_t>(player);
    entity ent_ret              = entt::null;
    item_menu_result_t menu_res = item_menu_result_t::NO_RESPONSE;
    // get_option
    auto opt                    = static_cast<size_t>(get_option());

    render_inventory(inventory, "inventory-drop");

    if(opt < inventory.items.size()) {
        menu_res = item_menu_result_t::SELECTED;
        auto it  = std::next(inventory.items.begin(), opt);
        ent_ret  = it->second.back();
    } else if(opt == sf::Keyboard::Escape) {
        menu_res = item_menu_result_t::CANCEL;
    }

    return std::make_tuple(menu_res, ent_ret);
}

}  // namespace radl::gui
