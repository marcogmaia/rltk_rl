#pragma once

#include <memory>
#include <vector>
#include "common.hpp"
#include "libtcod.hpp"

struct tile_t {
    bool explored = false;
};


// class BspListener;
class Map : public ITCODBspCallback {
    // friend class BspListener;

    uint32_t room_num = 0;
    position_t last_pos{0, 0};


    const TCODColor darkWall{72 + 20, 72, 72};
    const TCODColor darkGround{120 + 20, 120, 120};

private:
    TCODMap map;
    TCODBsp bsp;

    std::vector<tile_t> tiles;
    std::vector<position_t> room_positions;

    int width;
    int height;

    void set_wall(position_t position);

    void dig_rect(rect_t rect);

    /**
     * @brief Create a square room given 2 positions and adds to
     * @a room_positions
     *
     */
    void create_room(rect_t rect);

    void create_corridor(position_t pos1, position_t pos2);


    bool visitNode(TCODBsp* node, void* userData = nullptr) override;

public:
    Map(int width, int height);
    ~Map();
    bool is_walkable(position_t position) const;
    bool is_explored(position_t pos);

    const std::vector<position_t>& get_room_positions() const;

    void render() const;
};
