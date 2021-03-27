#pragma once
#include "libtcod.hpp"
#include "vchar.hpp"

// To render anything we need to know the position, colors and console
// the position we get directly from the position_t struct of the entity
struct Renderable {
    TCODConsole* console;
    radl::vchar_t vchar;
};
