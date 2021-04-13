#include "component/inventory.hpp"
#include "core/factories.hpp"
#include "utils/colors.hpp"
#include "core/engine.hpp"


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
    vchar_t item_vch('!', YELLOW, BLACK);
    auto ent_item
        = engine::factory::item_factory("Healing potion", item, item_vch);
    reg.emplace<item_potion_t>(ent_item, item_potion_t{
                                             .healing = 10,
                                         });
    return ent_item;
}

void add_to_inventory(entity ent, entity item) {
    auto& inventory = reg.get<inventory_t>(ent);
    inventory.items.push_back(item);
}

}  // namespace radl::component
