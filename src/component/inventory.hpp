#pragma once
#include <list>
#include "entt/entity/registry.hpp"
#include "component/position.hpp"
#include "core/engine.hpp"

namespace radl::component {
using entt::entity;

enum item_type_t {
    NONE,
    POTION,
};

struct item_potion_t {
    int healing;
};

struct wants_to_pickup_item_t {
    entity picked_by;
    entity item;
};

struct item_characteristics_t {
    bool drinkable = false;  // can drink
    bool castable  = false;  // can cast
};

struct wants_to_use_t {
    entity what;
};
struct item_t {
    item_type_t type;
    item_characteristics_t characteristics;
    bool in_pack = false;
};

struct inventory_t {
    std::list<entt::entity> items;

    inline bool contains(entity ent) {
        auto f_iter = std::find(items.cbegin(), items.cend(), ent);
        return f_iter != items.cend();
    }

    inline item_t& get_item(entity ent) {
        return reg.get<item_t>(ent);
    }

    inline void remove_first(entity ent) {
        auto iter = std::ranges::find(items, ent);
        if(iter != items.end()) {
            items.erase(iter);
        }
    }
};

entity healing_potion(bool in_pack = false);

void add_to_inventory(entity ent, entity item);

}  // namespace radl::component
