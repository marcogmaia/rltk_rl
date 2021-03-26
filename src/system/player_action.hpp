#pragma once

#include "entt/entity/registry.hpp"
#include "libtcod.hpp"


namespace radl {

void process_input(entt::registry&, entt::entity& e, const TCOD_key_t &key);

}