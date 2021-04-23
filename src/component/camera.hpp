#pragma once

#include "component/position.hpp"

// FIXME camera isn't a component.
// ## this is part of the game state (?)

namespace radl::component {

struct camera_t {
    // position to center the camera
    position_t position;
    int width;
    int height;
    bool dirty = true;  // flag to tell if the camera needs update
    double frame_time;
#ifdef DEBUG
    bool reveal_map = false;
#endif
};

}  // namespace radl::component
