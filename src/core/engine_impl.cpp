#include "gsl/gsl_util"

#include "core/engine.hpp"
#include "spdlog/spdlog.h"

namespace radl {

namespace system {

void init_systems();

}

namespace engine {

// state
namespace {

bool window_focused = true;
std::array<bool, 7> mouse_button_pressed;
int mouse_x = 0;
int mouse_y = 0;

}  // namespace

class Engine::EngineImpl {
private:
    sf::RenderWindow* main_window;

public:
    EngineImpl() {
        this->main_window = rltk::get_window();
    }

    ~EngineImpl() = default;

    sf::RenderWindow& get_window() {
        return *main_window;
    }

    void set_mouse_button_state(const int button, const bool state) {
        gsl::at(mouse_button_pressed, button) = state;
    }

    void set_mouse_position(const int x, const int y) {
        mouse_x = x;
        mouse_y = y;
    }

    void screen_resize(const sf::Event& event) {
        switch(event.type) {
        case sf::Event::Resized: {
            auto [screen_width, screen_height] = event.size;
            if(event.size.width < 1024) {
                screen_width = 1024;
            }
            if(event.size.height < 768) {
                screen_height = 768;
            }
            main_window->setSize(sf::Vector2u(screen_width, screen_height));
            main_window->setView(sf::View(
                sf::FloatRect(0.F, 0.F, static_cast<float>(screen_width),
                              static_cast<float>(screen_height))));
            rltk::gui->on_resize(screen_width, screen_height);
            engine::event_queue.push_back(event);
        } break;
        default: break;
        }
    }

    void focus(const sf::Event& event) {
        switch(event.type) {
        case sf::Event::LostFocus: {
            //
        } break;
        case sf::Event::GainedFocus: {
            spdlog::debug("gained FOCUS");
        } break;
        default: break;
        }
    }
};

Engine::Engine() {
    init();
    system::init_systems();
}

Engine::~Engine() {
    terminate();
}

void Engine::init() {
    system::init_systems();
}

void Engine::set_kb_event(const sf::Event& event) {
    switch(event.type) {
    case sf::Event::KeyPressed: {
        event_dispatcher.enqueue(event);
    } break;
    case sf::Event::KeyReleased: {
        //
    } break;
    default: break;
    }
}

void Engine::set_mouse_event(const sf::Event& event) {
    // if(event.type ==sf::Event::mouseMove) {}
    switch(event.type) {
    case sf::Event::MouseMoved: {
        // set mouse position
        auto [x, y] = event.mouseMove;
        engine_impl->set_mouse_position(x, y);
    } break;
    case sf::Event::MouseButtonPressed: {
        engine_impl->set_mouse_button_state(event.mouseButton.button, true);
    } break;
    case sf::Event::MouseButtonReleased: {
        engine_impl->set_mouse_button_state(event.mouseButton.button, false);
    }
    default: break;
    }
}

void Engine::run_game() {
    state::reset_mouse_state();
    auto& main_window  = engine_impl->get_window();
    double duration_ms = 0.0;


    auto clock = std::chrono::steady_clock();
    while(main_window.isOpen()) {
        auto start_time = clock.now();

        sf::Event event;
        while(main_window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                main_window.close();
            }
            engine_impl->screen_resize(event);
            engine_impl->focus(event);
            set_mouse_event(event);
            set_kb_event(event);
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


}  // namespace engine


}  // namespace radl
