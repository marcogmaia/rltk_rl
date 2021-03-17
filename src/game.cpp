#include <iostream>
#include "libtcod.hpp"
#include "game.hpp"
#include "engine.hpp"


void game() {
    while(TCODConsole::isWindowClosed() == false) {
        engine::engine.render();
        TCODConsole::root->flush();
        engine::engine.update();
    }
}
