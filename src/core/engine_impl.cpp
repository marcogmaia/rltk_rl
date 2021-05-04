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

void init_systems();

}

namespace engine {

// state
namespace {

// bool window_focused = true;
std::array<bool, 7> mouse_button_pressed;
int mouse_x = 0;
int mouse_y = 0;
}  // namespace


#ifdef DEBUG

void imgui_process_event(const sf::Event& event) {
    if(event.type == sf::Event::EventType::KeyPressed
       || event.type == sf::Event::EventType::KeyReleased) {
        if(event.key.code < 0) {
            return;
        }
    }
    ImGui::SFML::ProcessEvent(event);
}

void imgui_frame(const sf::RenderTexture& texture) {
    if(!reg.valid(player) || !reg.all_of<player_t, combat_stats_t>(player)) {
        return;
    }
    static sf::Clock deltaClock;
    auto& main_window = *rltk::get_window();
    ImGui::SFML::Update(main_window, deltaClock.restart());

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
    // window not dockable into, because it would be confusing to have two
    // docking targets within each others.
    ImGuiWindowFlags window_flags
        = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                    | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags
        |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    // bool is_open;
    ImGui::Begin("dockspace", (bool*)nullptr, window_flags);
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id);
    ImGui::End();
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);


    ImGui::Begin("Add Items");
    auto pressed = ImGui::Button("+ healing potion.");
    if(pressed) {
        reg.get<inventory_t>(player).add_item(
            factory::items::potion_healing(true));
    }
    ImGui::End();

    ImGui::Begin("Stats");
    {
        // change defense
        {
            auto& player_stats = reg.get<combat_stats_t>(player);
            ImGui::Text("Set def:");
            ImGui::SameLine();
            if(ImGui::ArrowButton("##def_up", ImGuiDir_Up)) {
                ++player_stats.defense;
            }
            ImGui::SameLine(0.0, 0.0);
            if(ImGui::ArrowButton("##def_down", ImGuiDir_Down)) {
                --player_stats.defense;
            }
            ImGui::SameLine();
            ImGui::Text("%d", player_stats.defense);
        }
        // show position
        {
            const auto& [px, py] = reg.get<position_t>(player);
            auto pos_str         = fmt::format("position: ({}, {})", px, py);
            ImGui::Text(pos_str.c_str());
        }
        // camera
        {
            static auto& camera = reg.ctx<camera_t>();
            auto fps            = 1000.0 / camera.frame_time;
            ImGui::Text(
                fmt::format("time: {:.2f}, fps: {:.2f}", camera.frame_time, fps)
                    .c_str());
            if(ImGui::Checkbox("reveal map", &camera.reveal_map)) {
                camera.dirty = true;
            }
            ImGui::Checkbox("mouse map", &camera.custom_position);
            if(camera.custom_position) {
                auto [mx, my] = engine::engine.get_mouse_position();
                auto [fw, fh] = term(gui::UI_MOUSE)->get_font_size();
                mx /= fw;
                my /= fh;
                camera.position = {mx, my};
                camera.dirty    = true;
            }
        }
    }
    ImGui::End();

    ImGui::Begin("Game Window");
    ImGui::Image(texture);
    ImGui::End();

    ImGui::SFML::Render(main_window);
}

void imgui_init() {
    ImGui::SFML::Init(*rltk::get_window());
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    engine::engine.dispatcher.sink<sf::Event>().connect<imgui_process_event>();
    // gui->add_owner_layer(GOD_UI, 0, 0, 1024, 768, resize_main, imgui_frame);
}

#endif


class Engine::EngineImpl {
private:
    sf::RenderWindow* main_window;

public:
    EngineImpl() {
        constexpr auto font_file = "../assets";
        rltk::init(rltk::config_advanced(font_file, 1024, 768, "MaiaRL"));
#ifdef DEBUG
        spdlog::set_level(spdlog::level::debug);
#endif
        spdlog::info("Initializing engine.");
        reg.set<game_state_t>(game_state_t::PRE_RUN);
        gui::init();
        system::init_systems();
        this->main_window = rltk::get_window();
    }

    ~EngineImpl() {
        gui::terminate();
    }

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

    void reset_mouse_state() {
        std::fill(mouse_button_pressed.begin(), mouse_button_pressed.end(),
                  false);
        mouse_x = 0;
        mouse_y = 0;
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
            // engine::event_queue.push_back(event);
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
    engine_impl = std::make_unique<EngineImpl>();
    imgui_init();
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

/**
 * @brief get last absolute mouse position
 *
 * @return position_t
 */
position_t Engine::get_mouse_position() {
    return {mouse_x, mouse_y};
}

void Engine::run_game() {
    engine_impl->reset_mouse_state();

    auto& main_window = engine_impl->get_window();
    main_window.setFramerateLimit(140);

    double duration_ms = 0.0;

    auto clock = std::chrono::steady_clock();

    sf::RenderTexture rtex;
    rtex.create(1024, 768);

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
            dispatcher.enqueue<sf::Event>(event);
        }

        // engine.game_tick
        dispatcher.trigger<double>(duration_ms);
        dispatcher.update();

        main_window.clear();

        // TODO sync imgui image frame with mouse
        rtex.clear();
        rltk::gui->render(rtex);
        rtex.display();
        imgui_frame(rtex);

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
