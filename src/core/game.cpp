#include <iostream>
#include <fmt/format.h>

#include "entt/entt.hpp"

#include "rltk/rltk.hpp"
#include "core/game.hpp"
#include "core/engine.hpp"

namespace {

using namespace radl;
using engine::console;
// using engine::main_window;

}  // namespace


void run_game(entt::delegate<void(void)> on_game_tick) {
    auto* main_window = rltk::get_window();
    while(main_window->isOpen()) {
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
                // console->resize_pixels(event.size.width, event.size.height);
                rltk::gui->on_resize(event.size.width, event.size.height);
                

                engine::event_queue.push(event);
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
                engine::event_queue.push(event);
                //
            } break;
            case sf::Event::KeyReleased: {
                //
            } break;
            default:
                break;
            }
        }

        on_game_tick();

     
        main_window->clear();

        rltk::gui->render(*main_window);

        main_window->display();
        // render
    }
}

void game() {
    engine::init();
    entt::delegate<void(void)> delegate_run_game;
    delegate_run_game.connect<&engine::update>();
    run_game(delegate_run_game);

    engine::terminate();
}
