#pragma once
#include <memory>
#include <queue>
#include <list>

#include "SFML/Window.hpp"

#include "entt/entity/fwd.hpp"

#include "system/map/map.hpp"

#include "entt/entity/fwd.hpp"
#include "entt/signal/dispatcher.hpp"

#include "core/gui/gui.hpp"

namespace radl {

using entt::entity;
using entt::registry;

}  // namespace radl


namespace radl::engine {


extern rltk::virtual_terminal* console;

using namespace component;

class Engine {
private:
    class EngineImpl;
    std::queue<sf::Event> event_queue;

    std::unique_ptr<EngineImpl> engine_impl;
    std::unique_ptr<gui::RadlUI> ui;

public:
    Engine();
    ~Engine();

    entt::dispatcher dispatcher;

    void set_kb_event(const sf::Event& event);
    void set_mouse_event(const sf::Event& event);

    /**
     * @brief Get the keyboard event
     *
     * @param event - event output
     * @return true if event was written to @a event
     */
    bool get_kb_event(sf::Event& event);

    sf::Event get_mouse_event();

    position_t get_mouse_position();

    void game_tick();
    void run_game();
};

extern Engine engine;

}  // namespace radl::engine
