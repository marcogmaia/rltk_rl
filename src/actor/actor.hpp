#pragma once
#include <string>
#include <memory>

#include "libtcod.hpp"

#include "common.hpp"
#include "destructible.hpp"
#include "ai.hpp"
#include "attacker.hpp"


constexpr size_t max_actor_name_len = 64;

/**
 * @brief  An actor is anything that can perform an action on the game world
 *
 */
class Actor {
public:
    int ch;  // ascii code
    char name[max_actor_name_len];
    position_t position;
    uint32_t fov_radius;
    TCODColor color;

    bool blocks = true;  // can walk over this actor

    Attacker* attacker         = nullptr;  // something that deals damage
    Destructible* destructible = nullptr;  // something that can be damaged
    std::unique_ptr<Ai> ai;                // something self-updating


    Actor(const position_t& position, int ch = '@', const char* src_name = "",
          uint32_t fov_rad = 8, const TCODColor& color = TCODColor::white);

    //  copy constructor
    Actor(const Actor& rhs) = default;

    ~Actor();
    // ~Actor() = default;

    /**
     * @brief
     *
     * @return true if updated, false otherwise
     */
    bool update();


    void render() const;
};
