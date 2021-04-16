
add_library(
    game_core
    "engine.cpp"
    "factories.cpp"
    "gui/gui.cpp"
    "gui/item_menu.cpp"
    "map.cpp"
    "spawner.cpp"
)

target_link_libraries(
    game_core PRIVATE
    ImGui-SFML
    "game_component"
    "game_system"
    "game_utils"
)
    
target_link_libraries(
    game_core PUBLIC
    "fmt"
    "pthread"
    "rltk"
    "spdlog::spdlog"
    "tbb"
)
