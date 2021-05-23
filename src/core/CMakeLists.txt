include_directories("../../lib/imgui-sfml")

add_library(
    imgui_sfml
    "../../lib/imgui-sfml/imgui-SFML.cpp"
    "../../lib/imgui/imgui.cpp"
    "../../lib/imgui/imgui_widgets.cpp"
    "../../lib/imgui/imgui_tables.cpp"
    "../../lib/imgui/imgui_draw.cpp"
    "../../lib/imgui/imgui_demo.cpp"
)

target_link_libraries(imgui_sfml OpenGL)

add_library(
    game_core
    "engine.cpp"
    "gui/gui.cpp"
    "gui/item_menu.cpp"
)

target_link_libraries(
    game_core PRIVATE
    "game_component"
    "game_system"
    "fmt"
    "pthread"
    "rltk"
    "tbb"
)
    
target_link_libraries(
    game_core PUBLIC
    imgui_sfml
    "spdlog::spdlog"
    "game_utils"
)
