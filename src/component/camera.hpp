#pragma once

#include "component/position.hpp"

namespace radl::component {

struct camera_t {
    // position to center the camera
    position_t position;
    int width;
    int height;
    bool dirty = true;  // flag to tell if the camera needs update
    double frame_time;
    
    // for debugging purposes
    bool reveal_map      = false;
    bool custom_position = false;
};

}  // namespace radl::component
