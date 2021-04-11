#include <iostream>
#include <fmt/format.h>

#include "entt/entt.hpp"

#include "rltk/rltk.hpp"
#include "core/engine.hpp"

namespace {

using namespace radl;
using engine::console;
// using engine::main_window;

}  // namespace


void run_game(entt::delegate<void(double)> on_game_tick) {
    auto* main_window  = rltk::get_window();
    double duration_ms = 0.0;

    while(main_window->isOpen()) {
        // auto elapsed_time = sf::Clock();
        clock_t start_time = clock();

        sf::Event event;
        while(main_window->pollEvent(event)) {
            switch(event.type) {
            case sf::Event::Closed: {
                main_window->close();
            } break;
            case sf::Event::Resized: {
                main_window->setView(sf::View(sf::FloatRect(
                    0.f, 0.f, static_cast<float>(event.size.width),
                    static_cast<float>(event.size.height))));
                rltk::gui->on_resize(event.size.width, event.size.height);
                engine::event_queue.push_back(event);
            } break;
            case sf::Event::LostFocus: {
                //
            } break;
            case sf::Event::GainedFocus: {
                //
            } break;
            case sf::Event::MouseMoved: {
                //
            } break;
            case sf::Event::MouseButtonPressed: {
                //
            } break;
            case sf::Event::MouseButtonReleased: {
                //
            } break;
            case sf::Event::KeyPressed: {
                engine::event_queue.push_back(event);
                engine::event_dispatcher.enqueue<sf::Event>(event);
            } break;
            case sf::Event::KeyReleased: {
                //
            } break;
            default:
                break;
            }
        }

        if(on_game_tick) {
            on_game_tick(duration_ms);
        }

        main_window->clear();

        rltk::gui->render(*main_window);

        main_window->display();
        
        duration_ms = ((clock() - start_time) * 1000.0) / CLOCKS_PER_SEC;
    }
}

int main() {
    engine::init();

    entt::delegate<void(double)> delegate_run_game;
    delegate_run_game.connect<&engine::update>();

    run_game(delegate_run_game);

    engine::terminate();

    return 0;
}
