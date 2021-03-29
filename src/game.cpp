#include <iostream>
#include <fmt/format.h>
#include "rltk/rltk.hpp"
#include "game.hpp"
#include "engine.hpp"

void game() {
    radl::Engine engine;
    engine.init();
    auto tick = [&](double ms) {
        engine.update();
        // render needs to be the last thing with this toolkit
        engine.render();
    };
    rltk::run(tick);
}
