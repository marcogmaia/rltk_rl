#include <iostream>
#include <algorithm>
#include <ranges>
#include <fmt/format.h>

#include <utility>

#include "map.hpp"
#include "libtcod.hpp"

static constexpr int room_max_size = 12;
static constexpr int room_min_size = 5;

Map::Map(int width, int height)
    : map(width, height)
    , bsp(0, 0, width, height)
    , width(width)
    , height(height) {
    std::cout << "Map CTOR called\n";
    tiles.resize(width * height);
    bsp.splitRecursive(nullptr, 5, room_max_size, room_max_size, 1.5f, 1.5f);
    bsp.traverseLevelOrder(this, nullptr);

    // for each adjancent room, create a corridor
    auto first = room_positions.crbegin();
    auto last  = room_positions.crend();
    if(first != last) {
        auto trailer = first;
        ++first;
        for(; first != last; ++first) {
            create_corridor(*trailer, *first);
        }
    }

    std::cout << fmt::format("num of rooms: {}\n", room_positions.size());
}

Map::~Map() {
    std::cout << "Map DTOR called\n";
}

void Map::render() {
    for(int x = 0; x < width; ++x) {
        for(int y = 0; y < height; ++y) {
            if(is_in_fov({x, y})) {
                auto color = map.isWalkable(x, y) ? lightWall : lightGround;
                TCODConsole::root->setCharBackground(x, y, color);
            }
            else if(is_explored({x, y})) {
                auto color = map.isWalkable(x, y) ? darkGround : darkWall;
                TCODConsole::root->setCharBackground(x, y, color);
            }
        }
    }
}

bool Map::is_explored(position_t pos) const {
    return tiles[pos.x + pos.y * width].explored;
}

bool Map::is_in_fov(position_t pos) {
    if(map.isInFov(pos.x, pos.y)) {
        tiles[pos.x + pos.y * width].explored = true;
        return true;
    }
    return false;
}

bool Map::is_walkable(position_t pos) const {
    return map.isWalkable(pos.x, pos.y);
}

void Map::compute_fov(const Actor& player) {
    const auto& pos = player.position;
    map.computeFov(pos.x, pos.y, player.fov_radius);
}

void Map::dig_rect(rect_t rect) {
    if(rect.w == 0 || rect.h == 0) {
        // nothing to dig here
        return;
    }
    for(int xi = rect.x; xi < (rect.x + rect.w); ++xi) {
        for(int yi = rect.y; yi < (rect.y + rect.h); ++yi) {
            map.setProperties(xi, yi, true, true);
        }
    }
}

const std::vector<position_t>& Map::get_room_positions() const {
    return room_positions;
}

// return a vector of positions that centers each room
void Map::create_room(rect_t rect) {
    dig_rect(rect);
}

void Map::create_corridor(position_t pos1, position_t pos2) {
    if(pos1 > pos2)
        std::swap(pos1, pos2);
    int xi = std::min(pos1.x, pos2.x);
    int yi = std::min(pos1.y, pos2.y);
    int w  = std::abs(pos2.x - pos1.x);
    int h  = std::abs(pos2.y - pos1.y);

    dig_rect(rect_t{xi, yi, w, 1});
    if(yi < pos2.y) {
        dig_rect(rect_t{xi + w, yi, 1, h});
    }
    else {
        dig_rect(rect_t{xi, yi, 1, h});
    }
}

bool get_valid_rectangle(TCODBsp* node, rect_t* dst) {
    // 1. pego um subrect
    // 2. verifico se ele satisfaz o minsize
    // 3. retorno done, simples
    // 1:
    auto* rng = TCODRandom::getInstance();
    auto x    = rng->getInt(node->x, node->x + node->w);
    auto y    = rng->getInt(node->y, node->y + node->h);
    auto wf   = node->x + node->w - x;  // node->w - (x - node->x) - 3;
    auto hf   = node->y + node->h - y;  // node->h - (y - node->y) - 3;
    auto w    = rng->getInt(0, wf);
    auto h    = rng->getInt(0, hf);
    // 2:
    if(w < room_min_size || h < room_min_size)
        return false;
    // 3:
    *dst = rect_t{x, y, w, h};
    return true;
}

bool Map::visitNode(TCODBsp* node, void* userData) {
    if(node->isLeaf()) {
        rect_t rect;
        auto is_valid = false;
        while(!is_valid) {
            is_valid = get_valid_rectangle(node, &rect);
        }
        if(is_valid) {
            create_room(rect);
            room_positions.push_back(rect.center());
        }
    }
    return true;
}
