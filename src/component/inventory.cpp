#include "component/inventory.hpp"
#include "core/factories.hpp"
#include "core/engine.hpp"
#include "utils/colors.hpp"

namespace radl::component {

entity healing_potion(bool in_pack) {
    auto item = item_t {
                .type            = item_type_t::POTION,
                .characteristics = item_characteristics_t{
                    .drinkable = true,
                    .castable  = false,
                },
                .in_pack = in_pack,
            };
    vchar_t item_vch(glyph::POTION, LIGHTER_RED, BLACK);
    auto ent_item = factory::item_factory("Healing potion", item, item_vch);
    reg.emplace<drinkable_effects_t>(ent_item, drinkable_effects_t{
                                                   .healing = 10,
                                               });
    return ent_item;
}

void add_to_inventory(entity ent, entity item) {
    auto& inventory = reg.get<inventory_t>(ent);
    inventory.items.push_back(item);
}

std::list<item_t> inventory_t::get_items() {
    std::list<item_t> list_items;
    std::ranges::transform(items, std::back_inserter(list_items),
                           [&list_items](entity ent) {
                               return reg.get<item_t>(ent);
                           });
    return list_items;
}

void inventory_t::remove_first(entity ent) {
    auto iter = std::ranges::find(items, ent);
    if(iter != items.end()) {
        items.erase(iter);
    }
}

}  // namespace radl::component
