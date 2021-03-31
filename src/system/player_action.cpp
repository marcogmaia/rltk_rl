
#include "player_action.hpp"
#include "move_attack.hpp"
#include "SFML/System.hpp"
#include "spdlog/spdlog.h"
namespace radl {


static position_t get_next_position(const sf::Event& ev, bool* player_input) {
    int dx = 0;
    int dy = 0;
    if(ev.type != sf::Event::EventType::KeyPressed) {
        if(ev.type == sf::Event::Resized) {
            using rltk::console;
            console->dirty = true;
            spdlog::debug("screen resize: x: {}, y: {}", console->term_width,
                          console->term_height);
        }
        return {dx, dy};
    }

    using sf::Keyboard;
    switch(ev.key.code) {
    case Keyboard::Numpad8:
    case Keyboard::K: {
        --dy;
    } break;
    case Keyboard::Numpad2:
    case Keyboard::J: {
        ++dy;
    } break;
    case Keyboard::Numpad4:
    case Keyboard::H: {
        --dx;
    } break;
    case Keyboard::Numpad6:
    case Keyboard::L: {
        ++dx;
    } break;
    case Keyboard::Numpad7:
    case Keyboard::Y: {
        --dx;
        --dy;
    } break;
    case Keyboard::Numpad9:
    case Keyboard::U: {
        ++dx;
        --dy;
    } break;
    case Keyboard::Numpad1:
    case Keyboard::B: {
        --dx;
        ++dy;
    } break;
    case Keyboard::Numpad3:
    case Keyboard::N: {
        ++dx;
        ++dy;
    } break;
    case sf::Keyboard::Numpad5: {
        *player_input = true;
    } break;
    default:
        break;
    }

    if(!*player_input) {
        *player_input = dx != 0 || dy != 0;
    }
    return position_t{dx, dy};
}

bool process_input(entt::registry& reg, entt::entity& e) {
    bool player_input = false;
    using rltk::my_event_queue;
    while(!my_event_queue.empty()) {
        auto ev = my_event_queue.front();
        my_event_queue.pop();
        auto delta_pos = get_next_position(ev, &player_input);
        move_attack(reg, e, delta_pos);
    }
    return player_input;
}

}  // namespace radl
