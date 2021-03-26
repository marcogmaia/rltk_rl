#include "position.hpp"
#include "player_action.hpp"

namespace radl {


static position_t get_next_position(const TCOD_key_t& key) {
    int dy = 0;
    int dx = 0;
    switch(key.vk) {
    case TCODK_KP8:
    case TCODK_UP: {
        --dy;
    } break;
    case TCODK_KP2:
    case TCODK_DOWN: {
        ++dy;
    } break;
    case TCODK_KP4:
    case TCODK_LEFT: {
        --dx;
    } break;
    case TCODK_KP6:
    case TCODK_RIGHT: {
        ++dx;
    } break;
    case TCODK_KP7: {
        --dx;
        --dy;
    } break;
    case TCODK_KP9: {
        ++dx;
        --dy;
    } break;
    case TCODK_KP1: {
        --dx;
        ++dy;
    } break;
    case TCODK_KP3: {
        ++dx;
        ++dy;
    } break;
    case TCODK_KP5: {
    };
    default:
        break;
    }
    return position_t{dx, dy};
}

void process_input(entt::registry& reg, entt::entity& e, const TCOD_key_t& key) {
    auto dpos = get_next_position(key);

    auto& entpos = reg.get<position_t>(e);
    entpos += dpos;
}

}  // namespace radl