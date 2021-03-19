#include <iostream>

#include "actor.hpp"

Actor::Actor(const position_t& position, int ch, uint32_t fov_rad,
             const TCODColor& color)
    : position(position)
    , fov_radius(fov_rad)
    , ch(ch)
    , color(color) {
    // #ifdef DEBUG
    //     std::cout << "Actor CTOR called\n";
    // #endif
}

Actor::~Actor() {
    // #ifdef DEBUG
    //     std::cout << "Actor DTOR called\n";
    // #endif
}

void Actor::render() const {
    TCODConsole::root->setChar(position.x, position.y, ch);
    TCODConsole::root->setCharForeground(position.x, position.y, color);
}