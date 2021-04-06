#include "SFML/System.hpp"
#include "spdlog/spdlog.h"

#include "player_action.hpp"
#include "core/engine.hpp"

#include "utils/rng.hpp"
#include "utils/geometry.hpp"
#include "component/component.hpp"
#include "system/camera.hpp"

namespace radl {

void handle_screen_resize(const sf::Event& ev, entt::registry& reg,
                          entt::entity e) {
    if(ev.type == sf::Event::Resized) {
        using rltk::console;
        camera_update(reg, e);
        spdlog::debug("screen resize: x: {}, y: {}", console->term_width,
                      console->term_height);
    }
}

static position_t get_delta_pos(const sf::Event& ev) {
    position_t delta_pos{0, 0};
    auto& [dx, dy] = delta_pos;

    if(ev.type != sf::Event::EventType::KeyPressed) {
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
    } break;
    default:
        break;
    }

    return delta_pos;
}


static position_t render_pos(int rx, int ry) {
    using engine::reg;
    using rltk::console;
    auto [px, py] = reg.get<position_t>(engine::player);
    auto xci      = px - console->term_width / 2;
    auto yci      = py - console->term_height / 2;

    return position_t{rx - xci, ry - yci};
}


namespace {

using engine::reg;
using namespace world;
using namespace rltk::colors;

void draw_line_from_player(sf::Event& ev, const position_t& player_pos) {
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

bool process_input(entt::entity ent) {
    using rltk::my_event_queue;

    if(!engine::event_queue.empty()) {
        auto ev = engine::event_queue.front();
        engine::event_queue.pop();

        handle_screen_resize(ev, reg, ent);

        auto player_pos = reg.get<position_t>(engine::player);

        switch(ev.type) {
        case sf::Event::KeyPressed: {
            auto target_pos = player_pos + get_delta_pos(ev);
            move_wait_attack(ent, target_pos);
            return true;
        } break;
        case sf::Event::MouseMoved: {
            draw_line_from_player(ev, player_pos);
        } break;
        default:
            break;
        }
    }
    return false;
}

namespace {

std::mutex walk_mutex;

}  // namespace

void walk(const entt::entity& ent, const position_t& src_pos,
          const position_t& target_pos) {
    if(!engine::get_map().at(target_pos).characteristics.walkable) {
        return;
    }

    std::lock_guard guard(walk_mutex);
    if(!world::is_occupied(reg, target_pos) && (src_pos != target_pos)) {
        world::map_entity_walk(ent, src_pos, target_pos);
        // mark viewshed as dirty to recalculate
        reg.get<world::viewshed_t>(ent).dirty = true;
    }
}

void random_walk(const entt::entity& ent, const position_t& src_pos) {
    auto dx = rng::rng.range(-1, 1);
    auto dy = rng::rng.range(-1, 1);

    position_t target_pos = src_pos + position_t{dx, dy};
    walk(ent, src_pos, target_pos);
}

bool move_wait_attack(entt::entity& ent, const position_t& dst_pos) {
    using namespace world;
    auto& src_pos = reg.get<position_t>(ent);
    // ## 0. wait turn
    if(dst_pos == src_pos) {
        return false;
    }
    auto& map                     = engine::get_map();
    const auto& target_tile_chars = map[{dst_pos}].characteristics;

    // ## 1. attack if enemy is in the targeted pos
    if(is_occupied(reg, dst_pos)) {
        // attack
        attack(ent, dst_pos);
        return false;
    }
    // ## 2. walk if tile is no occupied and walkable
    else if(target_tile_chars.walkable) {
        walk(ent, src_pos, dst_pos);
        return true;
    }
    // ## 3. do nothing if is wall
    return false;
}

}  // namespace radl
