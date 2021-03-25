#pragma once
#include <string>
#include <memory>

#include "libtcod.hpp"

#include "common.hpp"
#include "destructible.hpp"
#include "ai.hpp"
#include "attacker.hpp"


constexpr size_t max_entity_name_len = 64;

/**
 * @brief  An entity is anything that can perform an action on the game world
 *
 */
class Entity {
public:
    position_t position;
    int ch;  // ascii code
    std::string name;
    // char name[max_entity_name_len];
    uint32_t fov_radius;
    TCODColor color;

    bool blocks = true;  // can walk over this entity

    std::unique_ptr<Attacker> attacker;  // something that deals damage

    // destructible methods
    std::unique_ptr<Destructible>
        destructible;  // something that can be damaged
    bool is_dead();

    std::unique_ptr<Ai> ai;  // something self-updating

    Entity(const position_t& position, int ch = '@', const char* src_name = "",
           uint32_t fov_rad = 8, const TCODColor& color = TCODColor::white);
           
    Entity(const position_t& position, int ch, const std::string& src_name,
           uint32_t fov_rad, const TCODColor& color);

    //  copy constructor
    Entity(const Entity& rhs) = delete;

    ~Entity();
    // ~Entity() = default;

    /**
     * @brief
     *
     * @return true if updated, false otherwise
     */
    bool update();


    void render() const;
};
