#include <iostream>
#include <fmt/format.h>
#include "rltk/rltk.hpp"
#include "core/game.hpp"
#include "core/engine.hpp"

using namespace radl;

void run() {
    using rltk::console;
    sf::Event event;
    auto main_window = rltk::get_window();
    main_window->setFramerateLimit(240);

    while(main_window->isOpen()) {
        while(main_window->pollEvent(event)) {
            engine::event_queue.push(event);
            if(event.type == sf::Event::Closed) {
                main_window->close();
            }
            else if(event.type == sf::Event::Resized) {
                main_window->setView(sf::View(sf::FloatRect(
                    0.f, 0.f, static_cast<float>(event.size.width),
                    static_cast<float>(event.size.height))));
                console->resize_pixels(event.size.width, event.size.height);
                console->dirty = true;
            }
        }

        main_window->clear();
        console->render(*main_window);
        main_window->display();

        engine::update();
    }
}

void game() {
    engine::init();
    run();
}
