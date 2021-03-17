#include <iostream>

#include "actor.hpp"

Actor::Actor(position_t position, int ch, const TCODColor& color)
    : position(position)
    , ch(ch)
    , color(color) {
#ifdef DEBUG
    std::cout << "Actor CTOR called\n";
#endif
}

Actor::~Actor() {
#ifdef DEBUG
    std::cout << "Actor DTOR called\n";
#endif
}

void Actor::render() const {
    TCODConsole::root->setChar(position.x, position.y, ch);
    TCODConsole::root->setCharForeground(position.x, position.y, color);
}