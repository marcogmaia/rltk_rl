#include <iostream>
#include <chrono>
#include <fmt/format.h>

#include "spdlog/spdlog.h"

#include "entt/signal/dispatcher.hpp"

#include "rltk/rltk.hpp"
#include "core/engine.hpp"

namespace {

using namespace radl;
using engine::console;

}  // namespace

void run_game() {
    auto& main_window = *rltk::get_window();
    state::reset_mouse_state();
    double duration_ms = 0.0;


    auto clock = std::chrono::steady_clock();
    while(main_window.isOpen()) {
        auto start_time = clock.now();

        sf::Event event;
        while(main_window.pollEvent(event)) {
            engine::event_dispatcher.enqueue<sf::Event>(event);

            switch(event.type) {
            case sf::Event::Closed: {
                main_window.close();
            } break;
            case sf::Event::Resized: {
                if(event.size.width < 1024) {
                    event.size.width = 1024;
                }
                if(event.size.height < 768) {
                    event.size.height = 768;
                }
                main_window.setSize(
                    sf::Vector2u(event.size.width, event.size.height));
                main_window.setView(sf::View(sf::FloatRect(
                    0.F, 0.F, static_cast<float>(event.size.width),
                    static_cast<float>(event.size.height))));
                rltk::gui->on_resize(event.size.width, event.size.height);
                engine::event_queue.push_back(event);
            } break;
            case sf::Event::LostFocus: {
                //
            } break;
            case sf::Event::GainedFocus: {
                spdlog::debug("gained FOCUS");
            } break;
            case sf::Event::MouseMoved: {
                //
                state::set_mouse_position(event.mouseMove.x, event.mouseMove.y);
            } break;
            case sf::Event::MouseButtonPressed: {
                //
            } break;
            case sf::Event::MouseButtonReleased: {
                //
            } break;
            case sf::Event::KeyPressed: {
                // engine.set_kb_event(event)
                engine::event_queue.push_back(event);
            } break;
            case sf::Event::KeyReleased: {
                //
            } break;
            default: break;
            }
        }

        // engine.game_tick
        engine::event_dispatcher.trigger<double>(duration_ms);
        engine::event_dispatcher.update();

        main_window.clear();

        rltk::gui->render(main_window);

        main_window.display();

        duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          clock.now() - start_time)
                          .count();
    }
}


int main() {
    engine::init();
    run_game();
    engine::terminate();

    return 0;
}
