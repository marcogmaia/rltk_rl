#include "component/component.hpp"
#include "core/spawner.hpp"
#include "core/factories.hpp"

namespace radl {


void add_enemy(const position_t& pos) {
    if(!is_occupied(pos)) {
        entity e_ent;
        auto chance = rng::rng.range(1, 3);
        if(chance == 1) {
            e_ent = factory::enemy_factory(pos, vchar_t{'g', DARK_GREEN, BLACK},
                                           name_t{"Goblin", "Dead Goblin"});
        } else {
            e_ent = factory::enemy_factory(pos, vchar_t{'O', GREEN, BLACK},
                                           name_t{"Orc", "Dead Orc"});
        }
        auto chance_to_have_item
            = rng::rng.range(1, 10) == 1;  // 1 chance in 10
        if(chance_to_have_item) {
            auto& e_inventory = reg.get<inventory_t>(e_ent);
            e_inventory.add_item(healing_potion(true));
        }
    }
}

void add_enemies() {
    constexpr uint32_t max_enemies_pex_room = 5;
    using rng::rng;

    const auto& w_map = engine::get_map();
    for(const auto& room : w_map.rooms) {
        auto num_enemies = rng.range(0, max_enemies_pex_room);
        for(int i = 0; i < num_enemies; ++i) {
            enemy_spawner(room);
        }
    }
}

[[maybe_unused]] void enemy_spawner(rect_t rect) {
    auto random_pos = rect.random_pos();
    add_enemy(random_pos);
}


}  // namespace radl
