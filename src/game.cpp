#include <iostream>
#include <fmt/format.h>
#include "libtcod.hpp"
#include "game.hpp"
#include "engine.hpp"


void game() {
    engine::init();
    while(!TCODConsole::isWindowClosed()) {
        engine::render();
        TCODConsole::flush();
        engine::update();
    }

    std::cout << fmt::format("ran for {} seconds\n",
                             TCODSystem::getElapsedSeconds());
}
