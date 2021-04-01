#include <iostream>
#include <fmt/format.h>
#include "rltk/rltk.hpp"
#include "game.hpp"
#include "engine.hpp"

void game() {
    using namespace radl;
    engine::init();
    auto tick = [&](double ms) {
        engine::update();
        // render needs to be the last thing with this toolkit
        // engine.render();
    };
    rltk::run(tick);
}
