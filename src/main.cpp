#include <iostream>
#include <chrono>
#include <fmt/format.h>

#include "spdlog/spdlog.h"

#include "entt/signal/dispatcher.hpp"

#include "rltk/rltk.hpp"
#include "core/engine.hpp"

namespace {

using namespace radl;
using engine::console;

}  // namespace

int main() {
    engine::Engine engine;
    engine.init();
    engine.run_game();
    engine.terminate();

    // engine::init();
    // run_game();
    // engine::terminate();

    return 0;
}
