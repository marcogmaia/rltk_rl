#pragma once
#include "libtcod.hpp"

#include "common.hpp"

class Actor {
public:
    position_t position;
    int ch;  // ascii code
    TCODColor color;

    Actor(position_t position, int ch, const TCODColor &col);
    ~Actor();
    void render() const;
};
