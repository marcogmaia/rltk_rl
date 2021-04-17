#pragma once


namespace radl::component {

// a particle entity must contain a particle_t component and a renderable_t
struct particle_t {
    double lifetime_ms = 0.0;
};



}  // namespace radl::component
