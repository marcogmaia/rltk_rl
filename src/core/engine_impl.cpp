#include "core/engine.hpp"

namespace radl {

namespace system {

void init_systems();

}

namespace engine {

void Engine::init() {
    system::init_systems();
}

}  // namespace engine

}  // namespace radl
