#pragma once
#include <list>
#include "entt/entity/registry.hpp"
#include "component/position.hpp"
#include "core/engine.hpp"

namespace radl::component {
using entt::entity;

enum class item_type_t {
    NONE,
    POTION,
};

enum class item_id_t {
    NONE = 0,
    POTION_HEALING,
};

struct drinkable_effects_t {
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

struct wants_to_drop_t {
    entity what;
};

struct item_t {
    item_type_t type;
    item_id_t id;
    item_characteristics_t characteristics;
    bool in_pack = false;
};

struct inventory_t {
    std::map<item_id_t, std::vector<entt::entity>> items;

    inline bool contains(entity ent) {
        const auto& item_id = reg.get<item_t>(ent).id;
        if(!items.contains(item_id)) {
            return false;
        }
        bool found
            = std::ranges::find(items[item_id], ent) != items[item_id].end();
        return found;
    }

    inline item_t& get_item(entity ent) {
        return reg.get<item_t>(ent);
    }

    void add_item(entity ent_item);

    /**
     * @brief remove an entity item from the inventory
     *
     * @param ent
     */
    void remove_item(entity ent);

    /**
     * @brief get a vector of all entities in the inventory
     *
     * @return std::vector<entity>
     */
    std::vector<entity> get_items() const;

    std::vector<std::string> get_unique_item_names() const;
};


void add_to_inventory(entity ent, entity item);


namespace items {

entity potion_healing(bool in_pack = false);

}

}  // namespace radl::component
