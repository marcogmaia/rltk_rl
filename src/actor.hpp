#pragma once
#include "libtcod.hpp"

#include "common.hpp"

class Actor {
public:
    position_t position;
    uint32_t fov_radius;
    int ch;  // ascii code
    TCODColor color;

    Actor(const position_t& position, int ch = '@', uint32_t fov_rad = 8,
          const TCODColor& color = TCODColor::white);
    ~Actor();
    void render() const;
};
