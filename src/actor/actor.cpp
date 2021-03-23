#include <iostream>
#include <fmt/format.h>
#include <algorithm>

#include "actor.hpp"
#include "engine.hpp"

Actor::Actor(const position_t& position, int ch, const char* src_name,
             uint32_t fov_rad, const TCODColor& color)
    : ch(ch)
    , position(position)
    , name(src_name)
    , fov_radius(fov_rad)
    , color(color) {
    std::cout << fmt::format("Actor CTOR {}\n", name);
}

Actor::~Actor() {
    std::cout << "Actor DTOR\n";
}

bool Actor::update() {
    auto updated = false;
    if(ai) {
        updated = ai->update(this);
        if(updated) {
            std::cout << fmt::format("updating {}\n", name);
        }
    }
    return updated;
}

void Actor::die() {
    if(!destructible) {
        std::cout << fmt::format("The {} is though as f*ck\n", name);
        return;
    }
    destructible->die(this);
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
