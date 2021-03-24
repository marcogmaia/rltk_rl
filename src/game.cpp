#include <iostream>
#include <fmt/format.h>
#include "libtcod.hpp"
#include "game.hpp"
#include "engine.hpp"
#include <SDL2/SDL.h>


void game() {
    engine::init();
    while(!TCODConsole::isWindowClosed()) {
        engine::update();
        engine::render();
        auto& console = *TCODConsole::root;
        console.flush();
    }

    std::cout << fmt::format("ran for {} seconds\n",
                             TCODSystem::getElapsedSeconds());
}
