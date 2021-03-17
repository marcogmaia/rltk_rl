#pragma once
#include <vector>
#include "common.hpp"

struct tile_t {
    bool walkable = true;  // can we walk through this tile?
};

class Map {
private:
    std::vector<tile_t> tiles;
    void set_wall(position_t position);

public:
    int width;
    int height;
    Map(int width, int height);
    ~Map();
    bool is_wall(position_t position) const;
    bool is_walkable(position_t position) const;
    void render() const;
};
