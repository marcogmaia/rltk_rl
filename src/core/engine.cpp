#include "SFML/Window.hpp"
#include "gsl/gsl_util"

#include "core/engine.hpp"
#include "spdlog/spdlog.h"

#include "rltk/rltk.hpp"
#include "entt/entity/registry.hpp"

#include "system/game_state.hpp"
#include "core/gui/gui.hpp"

#include "imgui.h"
#include "imgui-SFML.h"
#include "component/component.hpp"
#include "system/factories.hpp"
#include "core/gui/gui.hpp"

namespace radl {

namespace system {

void system_camera();
void init_systems(engine::Engine& engine);

}  // namespace system

namespace engine {

using namespace rltk::colors;
rltk::virtual_terminal* console;

namespace {

using rltk::gui;
using rltk::term;

}  // namespace

// state
namespace {

// std::array<bool, 7> mouse_button_pressed;
int mouse_x = 0;
int mouse_y = 0;

}  // namespace

Engine::Engine() {
    constexpr auto font_file = "../assets";
    rltk::init(rltk::config_advanced(font_file, 1360, 768, "MaiaRL"));
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    spdlog::info("Initializing engine.");
    reg.set<game_state_t>(game_state_t::PRE_RUN);
    system::init_systems(*this);
    ui = std::make_unique<gui::RadlUI>();
}

Engine::~Engine() {}

void Engine::set_kb_event(const sf::Event& event) {
    switch(event.type) {
    case sf::Event::KeyPressed: {
        event_queue.push(event);
    } break;
    case sf::Event::KeyReleased: {
        //
    } break;
    default: break;
    }
}

std::queue<sf::Event> Engine::event_queue;

bool Engine::get_kb_event(sf::Event& event) {
    if(event_queue.empty()) {
        return false;
    }
    event = event_queue.front();
    event_queue.pop();
    return true;
}

void Engine::set_mouse_event(const sf::Event& event) {
    // if(event.type ==sf::Event::mouseMove) {}
    switch(event.type) {
    case sf::Event::MouseMoved: {
        // set mouse position
        // auto [x, y] = event.mouseMove;
        // engine_impl->set_mouse_position(x, y);
    } break;
    case sf::Event::MouseButtonPressed: {
        // engine_impl->set_mouse_button_state(event.mouseButton.button, true);
    } break;
    case sf::Event::MouseButtonReleased: {
        // engine_impl->set_mouse_button_state(event.mouseButton.button, false);
    }
    default: break;
    }
}

/**
 * @brief get last absolute mouse position
 *
 * @return position_t
 */
position_t Engine::get_mouse_position() {
    return {mouse_x, mouse_y};
}

void Engine::run_game() {
    // engine_impl->reset_mouse_state();

    auto& main_window = *rltk::get_window();
    main_window.setFramerateLimit(140);

    double duration_ms = 0.0;

    auto clock = std::chrono::steady_clock();

    while(main_window.isOpen()) {
        auto start_time = clock.now();

        sf::Event event;
        while(main_window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                main_window.close();
            }
            // engine_impl->screen_resize(event);
            // engine_impl->focus(event);
            set_mouse_event(event);
            set_kb_event(event);
            dispatcher.enqueue<sf::Event>(event);

            ui->process_event(event);
        }

        // engine.game_tick
        dispatcher.trigger<double>(duration_ms);
        dispatcher.update();

        main_window.clear();

        ui->render_ui();

        main_window.display();

        duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          clock.now() - start_time)
                          .count();
    }
}

void Engine::game_tick() {
    // TODO ? game_tick ?
}

}  // namespace engine


}  // namespace radl

