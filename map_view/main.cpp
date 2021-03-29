
// #include <iostream>
// #include <fmt/format.h>

// #include "rltk/rltk.hpp"


// // #include "map.hpp"


// static void charset_init() {
//     // auto constexpr* font_file = "../assets/tiles.png";
//     // auto constexpr* font_file = "../assets/terminal12x12_gs_ro.png";
//     auto constexpr* font_file = "../assets/terminal16x16_gs_ro.png";
//     TCOD_console_set_custom_font(
//         font_file, TCOD_FONT_LAYOUT_CP437 | TCOD_FONT_TYPE_GRAYSCALE, 16, 16);
//     TCOD_console_map_ascii_code_to_font('.', 10, 15);
// }

// static constexpr int screen_width  = 80;
// static constexpr int screen_height = 50;

// int main() {
//     charset_init();
//     TCODConsole::initRoot(screen_width, screen_height, "Maia Learning", false,
//                           TCOD_renderer_t::TCOD_RENDERER_SDL2);
//     TCODSystem::setFps(60);
//     // Map map(screen_width, screen_height);

//     auto& console = *TCODConsole::root;
//     console.setDefaultBackground(TCODColor::darkestGrey);

//     while(!TCODConsole::isWindowClosed()) {
//         TCOD_key_t key;
//         TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr);
//         TCODConsole::root->clear();
//         // map.test_render();
//         console.flush();
//     }


//     return 0;
// }

int main(){}