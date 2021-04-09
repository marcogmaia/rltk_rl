#pragma once
#include <vector>
#include "entt/entity/registry.hpp"

namespace radl::component {

enum item_type_t {
    NONE,
    POTION,
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

}  // namespace radl::component
