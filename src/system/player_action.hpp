#pragma once

#include "entt/entity/registry.hpp"
#include "libtcod.hpp"
#include "component/position.hpp"


namespace radl {

bool process_input(entt::registry&, entt::entity& e, const TCOD_key_t &key);

}