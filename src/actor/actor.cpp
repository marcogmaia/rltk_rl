#include <iostream>
#include <fmt/format.h>

#include "engine.hpp"
#include "actor.hpp"



Actor::Actor(const position_t& position, int ch, const char* src_name,
             uint32_t fov_rad, const TCODColor& color)
    : ch(ch)
    , position(position)
    , fov_radius(fov_rad)
    , color(color) {
    strncpy(name, src_name, sizeof name - 1);
    std::cout << fmt::format("Actor CTOR {}\n", name);
}

Actor::~Actor() {
    std::cout << "Actor DTOR\n";
}

void Actor::update() {
    std::cout << fmt::format("updating {}\n", name);
}

// how to make the Actor be self contained
bool Actor::move_attack(position_t pos) {
    auto& map = engine::map;
    if(!map->is_walkable(pos))
        return false;

    for(auto& actor : engine::actors) {
        if(this == actor.get())
            continue;
        if(pos == actor->position) {
            std::cout << fmt::format(
                "The {} laughs at your punny attempt to attack him!\n",
                actor->name);
            return false;
        }
    }
    this->position = pos;
    return true;
}

void Actor::render() const {
    TCODConsole::root->setChar(position.x, position.y, ch);
    TCODConsole::root->setCharForeground(position.x, position.y, color);
}