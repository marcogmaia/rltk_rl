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
        768,
    };

    gui->add_sparse_layer(UI_PARTICLES, map_rect.x1, map_rect.y1, map_rect.x2,
                          map_rect.y2, "16x16", resize_main);
    gui->add_layer(UI_MAP, map_rect.x1, map_rect.y1, map_rect.x2, map_rect.y2,
                   "16x16", resize_main, true);
    gui->add_layer(UI_ENTITIES, map_rect.x1, map_rect.y1, map_rect.x2,
                   map_rect.y2, "16x16", resize_main, false);
}


void RadlUI::render_ui() {
    if(!reg.valid(player) || !reg.all_of<player_t, combat_stats_t>(player)) {
        return;
    }
    static sf::Clock deltaClock;
    auto& main_window = *rltk::get_window();
    ImGui::SFML::Update(main_window, deltaClock.restart());
    // render UI
    render_game_window();
    // output window
    ImGui::EndFrame();
    ImGui::SFML::Render(*rltk::get_window());
}

namespace {

void handle_game_window_resize() {
    static ImVec2 last_size{0, 0};
    auto actual_size = ImGui::GetWindowSize();
    if(last_size.x != actual_size.x || last_size.y != actual_size.y) {
        last_size      = actual_size;
        auto& [ww, wh] = actual_size;
        rltk::gui->on_resize(ww, wh);
        if(reg.valid(player)) {
            auto* pcamera = reg.try_ctx<camera_t>();
            if(pcamera != nullptr) {
                system::system_camera();
                pcamera->dirty = true;
            }
        }
        spdlog::info("game window resized to {}x{}", actual_size.x,
                     actual_size.y);
    }
}

position_t get_coord_under_mouse(const ImVec2& window_start_pos,
                                 const ImVec2& window_size) {
    const auto& mouse_pos = ImGui::GetIO().MousePos;
    position_t mouse_coord{
        static_cast<int>(mouse_pos.x - window_start_pos.x) / 16,
        static_cast<int>(mouse_pos.y - window_start_pos.y) / 16};
    ImVec2 mid_window_pos{(std::floor(std::floor(window_size.x / 16) / 2.0F)),
                          std::floor(std::floor(window_size.y / 16) / 2.0F)};
    // mouse_coord - player_coord
    // show real coordinate on mouse
    const auto& player_coord = reg.get<position_t>(player);
    mouse_coord.first -= mid_window_pos.x;
    mouse_coord.second -= mid_window_pos.y;
    auto map_coord_under_mouse = player_coord + mouse_coord;
    return map_coord_under_mouse;
    return {-1, -1};
}

bool point_inside_rect(const ImVec2& pt, const ImVec2& start_pos,
                       const ImVec2& size) {
    auto is_inside = pt.x >= start_pos.x && pt.y >= start_pos.y
                     && pt.x <= (start_pos.x + size.x)
                     && pt.y <= (start_pos.y + size.y);
    return is_inside;
}

void mouse_overlay(const ImVec2& start_pos, const ImVec2& size) {
    auto& io        = ImGui::GetIO();
    auto& mouse_pos = io.MousePos;
    if(!point_inside_rect(mouse_pos, start_pos, size)) {
        return;
    }
    ImGui::SetNextWindowBgAlpha(0.4F);
    ImGui::SetNextWindowPos(io.MousePos, ImGuiCond_Always, {0, 1});
    static auto window_flags
        = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration
          | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav
          | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove;
    auto [coord_x, coord_y] = get_coord_under_mouse(start_pos, size);
    ImGui::Begin("##mouse_overlay", nullptr, window_flags);
    {
        ImGui::Separator();
        ImGui::Text("coord: (%d, %d)", coord_x, coord_y);
    }
    ImGui::End();
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
    auto& io                      = ImGui::GetIO();
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);
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
            ImGui::TextWrapped(fmt::format("window size: ({}, {})",
                                           io.DisplaySize.x, io.DisplaySize.y)
                                   .c_str());
            if(ImGui::Checkbox("reveal map", &camera.reveal_map)) {
                camera.dirty = true;
            }
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    {
        ImVec2 game_window_start_pos;
        ImVec2 game_window_size;
        ImGui::BeginChild("center", ImVec2{-192, 0}, false, game_window_flags);
        {
            ImGui::BeginChild(
                "##game_window",
                ImVec2{0, ImGui::GetWindowContentRegionMax().y * 0.8F}, false,
                game_window_flags);
            handle_game_window_resize();
            game_window_start_pos = ImGui::GetCursorScreenPos();
            ImGui::Image(m_game_window_texture);
            ImGui::EndChild();
            game_window_size = ImGui::GetItemRectSize();
            mouse_overlay(game_window_start_pos, game_window_size);
        }
        {
            ImGui::BeginChild(
                "##log1",
                ImVec2{0, ImGui::GetWindowContentRegionMax().y * 0.2F - 3.F},
                true);
            ImGui::Text("test text log");
            {
                auto map_coord_under_mouse = get_coord_under_mouse(
                    game_window_start_pos, game_window_size);
                ImGui::Text(fmt::format("mouse game coord: ({}, {})",
                                        map_coord_under_mouse.first,
                                        map_coord_under_mouse.second)
                                .c_str());
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    {
        ImGui::BeginChild("right", ImVec2{192.F - 10.0F, 0}, true);
        ImGui::Text("test text right");
        auto pressed = ImGui::Button("+ healing potion.");
        if(pressed) {
            reg.get<inventory_t>(player).add_item(
                factory::items::potion_healing(true));
        }
        ImGui::EndChild();
    }
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
    gui::init();
    ImGui::SFML::Init(*rltk::get_window());
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    m_game_window_texture.create(1920, 1080);
}

RadlUI::~RadlUI() {
    ImGui::SFML::Shutdown();
}


}  // namespace radl::gui
