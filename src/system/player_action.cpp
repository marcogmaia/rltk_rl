
#include "player_action.hpp"
#include "move_attack.hpp"

namespace radl {


static position_t get_next_position(const TCOD_key_t& key, bool* player_input) {
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
        *player_input = true;
    };
    default:
        break;
    }
    if(!*player_input) {
        *player_input = dx != 0 || dy != 0;
    }
    return position_t{dx, dy};
}

bool process_input(entt::registry& reg, entt::entity& e,
                   const TCOD_key_t& key) {
    bool player_input = false;
    auto dpos         = get_next_position(key, &player_input);
    move_attack(reg, e, dpos);
    // auto& entpos = reg.get<position_t>(e);
    // entpos += dpos;

    return player_input;
}

}  // namespace radl