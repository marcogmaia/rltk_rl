#include <iostream>

#include "actor.hpp"

Actor::Actor(const position_t& position, int ch, std::string name,
             uint32_t fov_rad, const TCODColor& color)
    : ch(ch)
    , name(name)
    , position(position)
    , fov_radius(fov_rad)
    , color(color) {
        std::cout << "Actor CTOR\n";
    }

    Actor::~Actor() {
        std::cout << "Actor DTOR\n";
    }

void Actor::render() const {
    TCODConsole::root->setChar(position.x, position.y, ch);
    TCODConsole::root->setCharForeground(position.x, position.y, color);
}