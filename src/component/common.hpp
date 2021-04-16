#pragma once

#include "entt/entity/registry.hpp"

using entt::entity;
using entt::registry;

namespace radl {

namespace engine {

extern entity player;
extern registry reg;

}  // namespace engine

}  // namespace radl

using radl::engine::reg;
using radl::engine::player;

