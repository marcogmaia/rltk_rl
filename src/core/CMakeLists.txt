
add_library(
    game_core
    "game.cpp"
    "engine.cpp"
    "map.cpp"
    "factories.cpp"
)

target_link_libraries(
    game_core PRIVATE
    game_utils
    game_system
    game_component
)
    
target_link_libraries(
    game_core PUBLIC
    spdlog::spdlog
    pthread
    tbb
    fmt
    rltk
)
