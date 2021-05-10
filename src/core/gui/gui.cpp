#include <algorithm>
#include <map>

#include "spdlog/spdlog.h"

#include "core/gui/gui.hpp"
#include "core/engine.hpp"

#include "component/component.hpp"
#include "system/factories.hpp"
#include "system/system.hpp"
#include "system/game_state.hpp"

#include "imgui.h"
#include "imgui_internal.h"
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

void RadlUI::render_ui() {
    if(!reg.valid(player) || !reg.all_of<player_t, combat_stats_t>(player)) {
        return;
    }
    static sf::Clock deltaClock;
    auto& main_window = *rltk::get_window();
    ImGui::SFML::Update(main_window, deltaClock.restart());


    render_game_window();
}

void RadlUI::display() {
    ImGui::EndFrame();
    ImGui::SFML::Render(*rltk::get_window());
}


namespace {

void handle_game_window_resize() {
    static ImVec2 last_size = ImGui::GetWindowSize();
    auto actual_size        = ImGui::GetWindowSize();
    if(last_size.x != actual_size.x || last_size.y != actual_size.y) {
        last_size      = actual_size;
        auto& [ww, wh] = actual_size;
        rltk::gui->on_resize(ww, wh);
        if(reg.valid(player)) {
            system::system_camera();
            reg.ctx<camera_t>().dirty = true;
        }
        spdlog::info("game window resized");
    }
}

}  // namespace

void RadlUI::render_game_window() {
    m_game_window_texture.clear();
    rltk::gui->render(m_game_window_texture);
    m_game_window_texture.display();


    static ImGuiWindowFlags game_window_flags
        = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
          | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration
          | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse
          | ImGuiWindowFlags_NoScrollbar
          | ImGuiWindowFlags_NoBringToFrontOnFocus;

    auto& io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::Begin("Game", nullptr, game_window_flags);
    {
        ImGui::BeginChild("left", {192, 0}, true);
        static float pad = 0.0;
        ImGui::Text("wtf");

        static bool show_metrics = false;
        ImGui::Checkbox("metric", &show_metrics);
        if(show_metrics) {
            ImGui::ShowMetricsWindow();
        }
        // ImGui::SetCursorPosX(pad);
        static bool show_demo_window = false;
        ImGui::Checkbox("demo", &show_demo_window);
        if(show_demo_window) {
            ImGui::ShowDemoWindow();
        }
        ImGui::InputFloat("padding", &pad);

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
            ImGui::TextWrapped(pos_str.c_str());
        }
        // camera
        {
            static auto& camera = reg.ctx<camera_t>();
            auto fps            = 1000.0 / camera.frame_time;
            ImGui::TextWrapped(
                fmt::format("time: {:.2f}, fps: {:.2f}", camera.frame_time, fps)
                    .c_str());
            ImGui::TextWrapped(fmt::format("window size: ({:.2f}, {:.2f})",
                                           io.DisplaySize.x, io.DisplaySize.y)
                                   .c_str());
            auto [tsizex, tsizey] = m_game_window_texture.getSize();
            if(ImGui::Checkbox("reveal map", &camera.reveal_map)) {
                camera.dirty = true;
            }
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    {
        ImGui::BeginChild("center", ImVec2{-192, 0}, false, game_window_flags);
        {
            ImGui::BeginChild(
                "##game_window",
                ImVec2{0, ImGui::GetWindowContentRegionMax().y * 0.8F}, false,
                game_window_flags);
            handle_game_window_resize();
            ImGui::Image(m_game_window_texture);
            ImGui::EndChild();
        }
        {
            ImGui::BeginChild(
                "##log1",
                ImVec2{0, ImGui::GetWindowContentRegionMax().y * 0.2F - 3.F},
                true);
            ImGui::Text("test text log");
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    {
        ImGui::BeginChild("right", ImVec2{192.F - 10.0F, 0}, true);
        ImGui::Text("test text right");
        ImGui::EndChild();
    }

    // ImGui::SetNextWindowPos({0 + 20, wpos.y + wsize.y - 20},
    // ImGuiCond_Always,
    //                         {0, 1});
    // {
    //     ImGui::BeginChild("Add Items", {256, 128});
    //     auto pressed = ImGui::Button("+ healing potion.");
    //     if(pressed) {
    //         reg.get<inventory_t>(player).add_item(
    //             factory::items::potion_healing(true));
    //     }
    //     ImGui::EndChild();
    // }

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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    m_game_window_texture.create(1920, 1080);
}

RadlUI::~RadlUI() {
    ImGui::SFML::Shutdown();
}


}  // namespace radl::gui
