
add_library(
    game_core
    "engine.cpp"
    "engine_impl.cpp"
    "gui/gui.cpp"
    "gui/item_menu.cpp"
    "../../lib/imgui/imgui_demo.cpp"
)

target_link_libraries(
    game_core PRIVATE
    ImGui-SFML
    "game_component"
    "game_system"
    "game_utils"
    "fmt"
    "pthread"
    "rltk"
    "tbb"
)
    
target_link_libraries(
    game_core PUBLIC
    "spdlog::spdlog"
)
