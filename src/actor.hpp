#pragma once
#include <string>
#include "libtcod.hpp"

#include "common.hpp"

class Actor {
public:
    int ch;  // ascii code
    std::string name;
    position_t position;
    uint32_t fov_radius;
    TCODColor color;


    Actor(const position_t& position, int ch = '@', std::string name = "",
          uint32_t fov_rad = 8, const TCODColor& color = TCODColor::white);

    //  copy constructor
    Actor(const Actor& rhs) = default;

    ~Actor();
    // ~Actor() = default;
    
    void render() const;
};
