#include <iostream>
#include <fmt/format.h>
#include "libtcod.hpp"
#include "game.hpp"
#include "engine.hpp"

void game() {
    Engine engine;
    engine.init();
    while(!TCODConsole::isWindowClosed()) {
        auto& console = *TCODConsole::root;
        engine.update();
        engine.render();
        console.flush();
    }

    std::cout << fmt::format("ran for {} seconds\n",
                             TCODSystem::getElapsedSeconds());
}
