#pragma once

#include "component/position.hpp"

namespace radl::component {

struct camera_t {
    position_t position;
    bool dirty = true;  // flag to tell if the camera needs update
#ifdef DEBUG
    bool reveal_map = false;
#endif
};

}  // namespace radl::component
