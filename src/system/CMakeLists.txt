
set(
    GAMESYS_SRC
    "ai.cpp"
    "camera.cpp"
    "combat.cpp"
    "factories.cpp"
    "player_action.cpp"
    "system.cpp"
    "visibility.cpp"
)
    
add_library(
    game_system
    ${GAMESYS_SRC}
)

target_link_libraries(
    game_system 
    game_component
    spdlog::spdlog
    pthread
    tbb
    fmt
)
