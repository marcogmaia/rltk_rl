
set(
    GAMESYS_SRC
    "ai.cpp"
    "camera.cpp"
    "combat.cpp"
    "factories.cpp"
    "game_state.cpp" 
    "map/dijkstra_map.cpp"
    "map/map.cpp"
    "system.cpp"
    "player_action.cpp"
    "visibility.cpp"
    "spawner.cpp"
)
    
add_library(
    game_system
    ${GAMESYS_SRC}
)

target_link_libraries(
    game_system 
    game_component
    game_utils
    spdlog::spdlog
    pthread
    tbb
    fmt
)
