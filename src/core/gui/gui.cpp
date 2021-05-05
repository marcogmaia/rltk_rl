#include <algorithm>
#include <map>

#include "spdlog/spdlog.h"

#include "core/gui/gui.hpp"
#include "core/engine.hpp"

#include "component/component.hpp"
#include "system/factories.hpp"
#include "system/game_state.hpp"

#include "imgui.h"
#include "imgui-SFML.h"


namespace radl::gui {

namespace {

using namespace rltk::colors;
using rltk::gui;

}  // namespace

void resize_main(rltk::layer_t* l, int w, int h) {
    // Simply set the width to the whole window width
    l->w = w;
    l->h = h;
}

void resize_inventory_popup(rltk::layer_t* l, int w, int h) {
    // Simply set the width to the whole window width
    l->w = l->w;
    l->h = l->h;
    l->x = 32;
    l->y = 32;
}

void resize_status(rltk::layer_t* l, int w, int h) {
    auto [fw, fh] = term(UI_STATUS)->get_font_size();

    l->w = (w / fw) * fw - (20 + 3) * fw;
    l->h = (2 + 8) * fh;
    l->y = ((h / fh) - (l->h / fh)) * fh;
}

void resize_inventory(rltk::layer_t* l, int w, int h) {
    auto [fw, fh] = term(UI_INVENTORY)->get_font_size();

    l->w = (20 + 2) * fw;
    l->h = (h / fh) * fh;
    l->x = ((w / fw) - (l->w / fw)) * fw;
    l->y = (h / fh - l->h / fh) * fh;
}


void init() {
    rect_t map_rect = {
        0,
        0,
        1024,
        768 - 16 * 10,
    };

    gui->add_layer(UI_MAP, map_rect.x1, map_rect.y1, map_rect.x2, map_rect.y2,
                   "16x16", resize_main, true);
    gui->add_layer(UI_ENTITIES, map_rect.x1, map_rect.y1, map_rect.x2,
                   map_rect.y2, "16x16", resize_main, false);
    gui->add_sparse_layer(UI_PARTICLES, map_rect.x1, map_rect.y1, map_rect.x2,
                          map_rect.y2, "16x16", resize_main);

    engine::console = term(UI_MAP);
}


void RadlUI::update() {
    if(!reg.valid(player) || !reg.all_of<player_t, combat_stats_t>(player)) {
        return;
    }
    static sf::Clock deltaClock;
    auto& main_window = *rltk::get_window();
    ImGui::SFML::Update(main_window, deltaClock.restart());
}

void RadlUI::frame() {
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
    static ImGuiWindowFlags dock_window_flags
        = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
          | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
          | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
          | ImGuiWindowFlags_NoBringToFrontOnFocus
          | ImGuiWindowFlags_NoNavFocus;
    // bool is_open;
    ImGui::Begin("dockspace", (bool*)nullptr, dock_window_flags);
    ImGuiID dockspace_id = ImGui::GetID("main_dockspace");
    ImGui::DockSpace(dockspace_id);
    ImGui::End();

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
    
    render_game_window();
}

void RadlUI::display() {
    ImGui::EndFrame();
    ImGui::SFML::Render(*rltk::get_window());
}

void RadlUI::render_game_window() {
    m_game_window_texture.clear();
    rltk::gui->render(m_game_window_texture);
    m_game_window_texture.display();

    static ImGuiWindowFlags game_window_flags
        = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
          | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse;
    static ImGuiWindowClass game_window_class;
    game_window_class.DockNodeFlagsOverrideSet
        = game_window_class.DockNodeFlagsOverrideClear
        = ImGuiDockNodeFlags_AutoHideTabBar;
    ImGui::SetNextWindowClass(&game_window_class);

    ImGui::Begin("Game Window", (bool*)nullptr, game_window_flags);
    {
        static bool first_run = true;
        static auto winsize   = ImGui::GetWindowSize();
        auto [w, h]           = ImGui::GetWindowSize();
        if(winsize.x != w || winsize.y != h || first_run) {
            first_run = false;
            winsize   = {w, h};
            m_game_window_texture.create(w, h);
            rltk::gui->on_resize(w, h);
        }
    }
    ImGui::Image(m_game_window_texture);
    ImGui::End();
}

void RadlUI::process_event(const sf::Event& event) {
    if(event.type == sf::Event::EventType::KeyPressed
       || event.type == sf::Event::EventType::KeyReleased) {
        if(event.key.code < 0) {
            return;
        }
    }
    ImGui::SFML::ProcessEvent(event);
}

RadlUI::RadlUI() {
    ImGui::SFML::Init(*rltk::get_window());
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // style:
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
}

RadlUI::~RadlUI() {
    ImGui::SFML::Shutdown();
}


}  // namespace radl::gui
