#pragma once

#include <memory>
#include <vector>

#include "libtcod.hpp"

#include "common.hpp"
#include "actor/actor.hpp"


struct tile_t {
    bool explored = false;
};


// class BspListener;
class Map : public ITCODBspCallback {
    // friend class BspListener;

    uint32_t room_num = 0;
    position_t last_pos{0, 0};


    const TCODColor darkWall{10 + 20, 10, 10};
    const TCODColor darkGround{50 + 20, 50, 50};

    const TCODColor lightWall{130, 110, 50};
    const TCODColor lightGround{200, 180, 50};

private:
    TCODMap map;
    TCODBsp bsp;

    std::vector<tile_t> tiles;
    std::vector<rect_t> rooms;

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

    void add_enemies_to_room(const rect_t& rect);

    void create_corridor(position_t pos1, position_t pos2);


    bool visitNode(TCODBsp* node, void* userData = nullptr) override;

public:
    Map(int width, int height);
    ~Map();

    bool pos_is_empty(position_t pos);

    bool is_wall(position_t position) const;

    bool can_walk(position_t pos) const;

    void set_property(position_t pos, bool transparent, bool walkable);

    bool is_explored(position_t pos) const;

    bool is_in_fov(position_t pos);

    void add_enemy(position_t pos);

    void compute_fov(const Actor& player);

    const std::vector<rect_t>& get_rooms() const;

    void render();
};
