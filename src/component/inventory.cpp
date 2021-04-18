#include "component/common.hpp"
#include "component/inventory.hpp"
#include "system/factories.hpp"
#include "core/engine.hpp"
#include "utils/colors.hpp"

namespace radl::component {

void add_to_inventory(entity ent, entity item) {
    auto& inventory     = reg.get<inventory_t>(ent);
    const auto& item_id = reg.get<item_t>(item).id;
    inventory.items[item_id].push_back(item);
}

void inventory_t::remove_item(entity ent) {
    const auto& item_id = reg.get<item_t>(ent).id;
    auto to_remove      = std::ranges::find(items[item_id], ent);
    items[item_id].erase(to_remove);
    if(items[item_id].empty()) {
        items.extract(item_id);
    }
}

void inventory_t::add_item(entity ent_item) {
    const auto& item_id = reg.get<item_t>(ent_item).id;
    items[item_id].push_back(ent_item);
}

std::vector<entity> inventory_t::get_items() const {
    std::vector<entity> vec;
    for(auto [item_id, item_bucket] : items) {
        std::ranges::copy(item_bucket, std::back_inserter(vec));
    }
    return vec;
}

std::vector<std::string> inventory_t::get_unique_item_names() const {
    std::vector<std::string> vec;
    for(auto [item_id, item_bucket] : items) {
        if(item_bucket.empty()) {
            continue;
        }
        auto item_name = reg.get<name_t>(item_bucket.back()).name;
        vec.push_back(item_name);
    }
    return vec;
}

}  // namespace radl::component
