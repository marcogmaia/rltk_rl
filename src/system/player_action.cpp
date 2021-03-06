#include "SFML/System.hpp"
#include "spdlog/spdlog.h"

#include "player_action.hpp"
#include "core/engine.hpp"

#include "utils/rng.hpp"
#include "utils/geometry.hpp"
#include "component/component.hpp"
#include "system/camera.hpp"
#include "system/combat.hpp"

namespace radl::system {

void handle_screen_resize(const sf::Event& ev, entt::registry& reg,
                          entt::entity e) {
    if(ev.type == sf::Event::Resized) {
        using engine::console;
        // camera_update(e);
        spdlog::debug("screen resize: x: {}, y: {}", console->term_width,
                      console->term_height);
    }
}

position_t get_delta_pos(const sf::Event& ev) {
    position_t delta_pos{0, 0};
    auto& [dx, dy] = delta_pos;

    if(ev.type != sf::Event::EventType::KeyPressed) {
        return {dx, dy};
    }

    using sf::Keyboard;
    switch(ev.key.code) {
    case Keyboard::Up:
    case Keyboard::Numpad8:
    case Keyboard::K: {
        --dy;
    } break;
    case Keyboard::Down:
    case Keyboard::Numpad2:
    case Keyboard::J: {
        ++dy;
    } break;
    case Keyboard::Left:
    case Keyboard::Numpad4:
    case Keyboard::H: {
        --dx;
    } break;
    case Keyboard::Right:
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
    } break;

    default: break;
    }

    return delta_pos;
}

namespace {

using namespace rltk::colors;

[[maybe_unused]] position_t render_pos(int rx, int ry) {
    using rltk::console;
    auto [px, py] = reg.get<position_t>(player);
    auto xci      = px - console->term_width / 2;
    auto yci      = py - console->term_height / 2;
    return position_t{rx - xci, ry - yci};
}

[[maybe_unused]] void draw_line_from_player(sf::Event& ev,
                                            const position_t& player_pos) {
    if(ev.type == sf::Event::EventType::MouseMoved) {
        auto [mx, my] = ev.mouseMove;
        int tx        = std::round(static_cast<double>(mx) / 16.0);
        int ty        = std::round(static_cast<double>(my) / 16.0);

        auto [px, py] = player_pos;
        auto func     = [](int x, int y) {
            rltk::console->set_char(x, y,
                                    vchar_t{glyph::BLOCK1, ORANGE, BLACK});
        };
        auto [rx, ry] = render_pos(px, py);
        radl::line_func(rx, ry, tx, ty, func);
    }
}

}  // namespace


void pick_item_at(entity ent, position_t pos) {
    auto& ents_here = get_map().at(pos).entities_here;
    auto found      = std::ranges::find_if(ents_here, [](auto ent) {
        return reg.all_of<item_t>(ent);
    });
    if(found != ents_here.end()) {
        auto ent_item = *found;
        reg.emplace_or_replace<wants_to_pickup_item_t>(ent,
                                                       wants_to_pickup_item_t{
                                                           .picked_by = ent,
                                                           .item = ent_item,
                                                       });
    }
}



/**
 * @brief set the player event input
 *
 * @param ev input event
 * @return return the next game state after the input
 */
game_state_t player_input(const sf::Event& ev) {
    auto player_pos = reg.get<position_t>(player);

    switch(ev.type) {
    case sf::Event::KeyPressed: {
        switch(ev.key.code) {
        case sf::Keyboard::I: {
            return game_state_t::SHOW_INVENTORY;
        } break;
        case sf::Keyboard::D: {
            return game_state_t::SHOW_INVENTORY_DROP;
        } break;
        case sf::Keyboard::Q: {
        } break;
        case sf::Keyboard::G: {
            pick_item_at(player, player_pos);
        }
        default: break;
        }
        auto target_pos = player_pos + get_delta_pos(ev);
        move_wait_attack(player, target_pos);
    } break;

    case sf::Event::MouseMoved: {
    } break;
    default: {
        return game_state_t::AWAITING_INPUT;
    } break;
    }

    return game_state_t::PLAYER_TURN;
}

/**
 * @brief make a entity do a random walk
 *
 * @param reg register
 * @param ent entity
 * @param src_pos initial position
 */
void random_walk(registry& reg, const entt::entity& ent,
                 const position_t& src_pos) {
    auto dx               = rng::rng.range(-1, 1);
    auto dy               = rng::rng.range(-1, 1);
    position_t target_pos = src_pos + position_t{dx, dy};
    walk(reg, ent, src_pos, target_pos);
}

bool move_wait_attack(entt::entity& ent, const position_t& dst_pos) {
    auto& src_pos = reg.get<position_t>(ent);
    // ## 0. wait turn
    if(dst_pos == src_pos) {
        return false;
    }
    auto& map                     = get_map();
    const auto& target_tile_chars = map[{dst_pos}].props;
    // ## 1. attack if enemy is in the targeted pos
    if(is_occupied(dst_pos)) {
        // attack
        system::attack(ent, dst_pos);
        return false;
    }
    // ## 2. walk if tile is no occupied and walkable
    if(target_tile_chars.walkable) {
        walk(reg, ent, src_pos, dst_pos);
        return true;
    }
    // ## 3. do nothing if is wall
    return false;
}

void system_player() {
    sf::Event ev;
    auto& game_state = reg.ctx<game_state_t>();

    while(engine::Engine::get_kb_event(ev)) {
        // every player action may mutate the game state, all the other systems
        // makes use of the state
        game_state = player_input(ev);
    }
}



}  // namespace radl::system
