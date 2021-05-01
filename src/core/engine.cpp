#include <memory>

#include "spdlog/spdlog.h"


#include "core/engine.hpp"
#include "system/spawner.hpp"

#include "system/game_state.hpp"

#include "utils/rng.hpp"

#include "gsl/gsl_util"


// TODO remove this
#include "core/gui/gui.hpp"

namespace radl::engine {

Engine engine;

using namespace rltk::colors;

rltk::virtual_terminal* console;

namespace {

using rltk::gui;
using rltk::term;

}  // namespace

}

// namespace radl::state {

// bool window_focused = true;
// std::array<bool, 7> mouse_button_pressed;
// int mouse_x = 0;
// int mouse_y = 0;


// void set_mouse_button_state(const int button, const bool state) {
//     gsl::at(mouse_button_pressed, button) = state;
// }

// bool get_mouse_button_state(const int button) {
//     return gsl::at(mouse_button_pressed, button);
// }

// double scale_factor = 1.0;
// void set_mouse_position(const int x, const int y) {
//     mouse_x = static_cast<int>(x / scale_factor);
//     mouse_y = static_cast<int>(y / scale_factor);
// }

// position_t get_mouse_position() {
//     return {mouse_x, mouse_y};
// }

// position_t get_mouse_position_coord() {
//     auto [mx, my] = state::get_mouse_position();
//     auto [fw, fh] = term(gui::UI_MOUSE)->get_font_size();
//     mx /= fw;
//     my /= fh;
//     return {mx, my};
// }

// void reset_mouse_state() {
//     std::fill(mouse_button_pressed.begin(), mouse_button_pressed.end(), false);
//     mouse_x = 0;
//     mouse_y = 0;
// }

// }  // namespace radl::state
