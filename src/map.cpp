#include <iostream>
#include <algorithm>
#include <fmt/format.h>

#include <utility>

#include "map.hpp"
#include "libtcod.hpp"

static constexpr uint32_t room_max_size = 12;
static constexpr uint32_t room_min_size = 6;

Map::Map(int width, int height)
    : bsp(0, 0, width, height)
    , width(width)
    , height(height) {
    tiles.resize(width * height);
    bsp.splitRecursive(nullptr, 8, room_max_size, room_max_size, 1.5f, 1.5f);
    bsp.traverseInvertedLevelOrder(this, nullptr);

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
    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            auto color = is_wall({x, y}) ? darkWall : darkGround;
            TCODConsole::root->setCharBackground(x, y, color);
        }
    }
}

#include <cassert>

void Map::dig(position_t pos1, position_t pos2) {
    // swap position so that the top left is pos1
    if(pos1.x > pos2.x) {
        std::swap(pos1.x, pos2.x);
    }
    if(pos1.y > pos2.y) {
        std::swap(pos1.y, pos2.y);
    }
    // fill rectangle
    for(int x = pos1.x; x < pos2.x; ++x) {
        for(int y = pos1.y; y < pos2.y; ++y) {
            auto pos                  = x + y * width;
            static const auto max_pos = width * height - 1;
            if(pos > max_pos) {
                break;
            }
            tiles[x + y * width].walkable = true;
        }
    }
}

const std::vector<position_t>& Map::get_room_positions() const {
    return room_positions;
}

// return a vector of positions that centers each room
void Map::create_room(position_t pos1, position_t pos2) {
    dig(pos1, pos2);
    position_t center_pos = (pos1 + pos2) / 2;
    room_positions.push_back(center_pos);
}

void Map::create_corridor(position_t pos1, position_t pos2) {
    if(pos1.x > pos2.x) {
        std::swap(pos1.x, pos2.x);
    }
    if(pos1.y > pos2.y) {
        std::swap(pos1.y, pos2.y);
    }

    for(int x = pos1.x; x < pos2.x; ++x) {
        tiles[x + pos1.y * width].walkable = true;
    }
    for(int y = pos1.y; y < pos2.y; ++y) {
        tiles[pos1.x + y * width].walkable = true;
    }
}


std::pair<position_t, position_t> get_valid_rectangle(TCODBsp* node) {
    auto* rng     = TCODRandom::getInstance();
    auto w        = node->w;
    auto w_half   = w / 2;
    auto x        = node->x;
    auto y        = node->y;
    auto p1x      = rng->getInt(x, x + w_half);
    auto p1y      = rng->getInt(y, y + node->h);
    position_t p1 = {p1x, p1y};
    auto p2x      = rng->getInt(x + w_half, x + w);
    auto p2y      = rng->getInt(y, y + node->h);
    position_t p2 = {p2x, p2y};
    return std::make_pair(p1, p2);
}

// bool BspListener::visitNode(TCODBsp* node, void* userData) {
bool Map::visitNode(TCODBsp* node, void* userData) {
    // auto* map           = static_cast<Map*>(userData);
    static uint32_t cnt = 0;
    ++cnt;

    // BUG por algum motivo essa call tá crashando aleatoriamente (o if)
    if(node->isLeaf()) {
        auto rng = TCODRandom::getInstance();
        auto pos = get_valid_rectangle(node);
        create_room(pos.first, pos.second);
        // map->create_room(pos.first, pos.second);

        // last_pos = {x + w / 2, y + h / 2};
        last_pos = (pos.first + pos.second) / 2;
        if(room_num != 0) {
            // dig a corridor from last room
            auto center_pos
                = position_t{node->x + node->w, node->y + node->h} / 2;
            // map->create_corridor(last_pos, center_pos);
        }
        ++room_num;
    }

    return true;
    // std::cout << fmt::format("visited {} nodes\n", cnt);
}
