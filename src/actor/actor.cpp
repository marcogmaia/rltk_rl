#include <iostream>
#include <fmt/format.h>
#include <algorithm>

#include "actor.hpp"
#include "engine.hpp"

Actor::Actor(const position_t& position, int ch, const char* src_name,
             uint32_t fov_rad, const TCODColor& color)
    : ch(ch)
    , name(src_name)
    , position(position)
    , fov_radius(fov_rad)
    , color(color) {}

Actor::~Actor() {}

bool Actor::update() {
    auto updated = false;
    if(ai) {
        updated = ai->update(this);
    }
    return updated;
}

bool Actor::is_dead() {
    if(!destructible) {
        return false;
    }
    return destructible->is_dead();
}

void Actor::render() const {
    TCODConsole::root->setChar(position.x, position.y, ch);
    TCODConsole::root->setCharForeground(position.x, position.y, color);
}
