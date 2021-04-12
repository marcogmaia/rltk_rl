#pragma once
#include <vector>
#include "entt/entity/registry.hpp"
#include "component/position.hpp"

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
    position_t at_position;
};

struct item_characteristics_t {
    bool drinkable = false;  // can drink
    bool castable  = false;  // can cast
};

struct item_t {
    item_type_t type;
    item_characteristics_t characteristics;
    bool in_pack = false;
};

struct inventory_t {
    std::vector<entt::entity> items;
};

entity healing_potion(bool in_pack = false);

void add_to_inventory(entity ent, entity item);

}  // namespace radl::component
