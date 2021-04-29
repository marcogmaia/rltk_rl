#include <memory>

#include "spdlog/spdlog.h"


#include "core/engine.hpp"
#include "system/spawner.hpp"

#include "system/game_state.hpp"

#include "utils/rng.hpp"

#include "gsl/gsl_util"


// FIXME remove this
#include "core/gui/gui.hpp"

namespace radl::engine {

using namespace rltk::colors;
std::deque<sf::Event> event_queue;

rltk::virtual_terminal* console;

entt::dispatcher event_dispatcher{};
entt::sigh<sf::Event()> ev_signal{};
entt::sink ev_sink{ev_signal};

namespace {

using rltk::gui;
using rltk::term;

}  // namespace

static void rltk_init() {
    constexpr auto font_file = "../assets";
    rltk::init(
        rltk::config_advanced(font_file, 1024, 768, "MaiaRL"));
}


// void init() {
// #ifdef DEBUG
//     spdlog::set_level(spdlog::level::debug);
// #endif
//     spdlog::info("Initializing engine.");
//     event_dispatcher.sink<double>().connect<&update>();
//     reg.set<game_state_t>(game_state_t::PRE_RUN);
//     rltk_init();
//     gui::init();
// }

}

namespace radl::state {

bool window_focused = true;
std::array<bool, 7> mouse_button_pressed;
int mouse_x = 0;
int mouse_y = 0;


void set_mouse_button_state(const int button, const bool state) {
    gsl::at(mouse_button_pressed, button) = state;
}

bool get_mouse_button_state(const int button) {
    return gsl::at(mouse_button_pressed, button);
}

double scale_factor = 1.0;
void set_mouse_position(const int x, const int y) {
    mouse_x = static_cast<int>(x / scale_factor);
    mouse_y = static_cast<int>(y / scale_factor);
}

position_t get_mouse_position() {
    return {mouse_x, mouse_y};
}

position_t get_mouse_position_coord() {
    auto [mx, my] = state::get_mouse_position();
    auto [fw, fh] = term(gui::UI_MOUSE)->get_font_size();
    mx /= fw;
    my /= fh;
    return {mx, my};
}

void reset_mouse_state() {
    std::fill(mouse_button_pressed.begin(), mouse_button_pressed.end(), false);
    mouse_x = 0;
    mouse_y = 0;
}

}  // namespace radl::state
