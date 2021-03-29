#pragma once

#include "entt/entity/registry.hpp"
// #include "libtcod.hpp"
#include "rltk/rltk.hpp"
#include "component/position.hpp"


namespace radl {

bool process_input(entt::registry&, entt::entity& e);

}