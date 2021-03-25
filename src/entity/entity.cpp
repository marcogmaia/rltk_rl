#include <iostream>
#include <fmt/format.h>
#include <algorithm>

#include "entity.hpp"
#include "engine.hpp"

Entity::Entity(const position_t& position, int ch, const char* src_name,
               uint32_t fov_rad, const TCODColor& color)
    : position(position)
    , ch(ch)
    , name(src_name)
    , fov_radius(fov_rad)
    , color(color) {}

Entity::Entity(const position_t& position, int ch, const std::string& src_name,
               uint32_t fov_rad, const TCODColor& color)
    : position(position)
    , ch(ch)
    , name(src_name)
    , fov_radius(fov_rad)
    , color(color) {}

// Entity::Entity(const position_t& position, int ch, const char* src_name,
//                uint32_t fov_rad, const TCODColor& color)
//     : position(position)
//     , ch(ch)
//     , name(src_name)
//     , fov_radius(fov_rad)
//     , color(color) {}

Entity::~Entity() {}

bool Entity::update() {
    auto updated = false;
    if(ai) {
        updated = ai->update(this);
    }
    return updated;
}

bool Entity::is_dead() {
    if(!destructible) {
        return false;
    }
    return destructible->is_dead();
}

void Entity::render() const {
    TCODConsole::root->setChar(position.x, position.y, ch);
    TCODConsole::root->setCharForeground(position.x, position.y, color);
}
