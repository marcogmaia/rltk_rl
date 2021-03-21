#pragma once
#include <string>
#include "common.hpp"
#include "libtcod.hpp"



constexpr size_t max_actor_name_len = 64;

class Actor {
public:
    int ch;  // ascii code
    char name[max_actor_name_len];
    position_t position;
    uint32_t fov_radius;
    TCODColor color;


    Actor(const position_t& position, int ch = '@', const char* src_name = "",
          uint32_t fov_rad = 8, const TCODColor& color = TCODColor::white);

    //  copy constructor
    Actor(const Actor& rhs) = default;

    void update();

    /**
     * @brief move self or attack other actor
     *
     * @param pos position to move or attack
     * @return true if moved, false otherwise
     */
    bool move_attack(position_t pos);

    ~Actor();
    // ~Actor() = default;

    void render() const;
};
