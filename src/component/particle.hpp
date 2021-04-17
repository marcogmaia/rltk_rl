#pragma once

#include "component/position.hpp"
#include "core/engine.hpp"

namespace radl::component {

// a particle entity must contain a particle_t component and a renderable_t
struct particle_t {
    double lifetime_ms = 0.0;
};


enum class particle_type_t {
    ATTACK,
    HEALING,
};

void particle_create(particle_type_t type, double duration_ms,
                  const position_t& pos);


}  // namespace radl::component
