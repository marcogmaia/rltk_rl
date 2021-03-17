#include "map.hpp"
#include "libtcod.hpp"
#include <iostream>

Map::Map(int width, int height)
    : width(width)
    , height(height) {
    tiles.resize(width * height);
    set_wall({30, 22});
    set_wall({50, 22});
#ifdef DEBUG
    std::cout << "Map CTOR called\n";
#endif
}
Map::~Map() {
#ifdef DEBUG
    std::cout << "Map DTOR called\n";
#endif
}
void Map::set_wall(position_t pos) {
    auto index            = pos.x + width * pos.y;
    tiles[index].walkable = false;
}

bool Map::is_wall(position_t pos) const {
    return !tiles[pos.x + width * pos.y].walkable;
}

bool Map::is_walkable(position_t pos) const {
    auto is_walkable = tiles[pos.x + width * pos.y].walkable;
    return is_walkable;
}

void Map::render() const {
    static const TCODColor darkWall(0, 0, 100);
    static const TCODColor darkGround(50, 50, 150);
    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            auto color = is_wall({x, y}) ? darkWall : darkGround;
            TCODConsole::root->setCharBackground(x, y, color);
        }
    }
}
