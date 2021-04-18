
set(
    GAMESYS_SRC
    "camera.cpp"
    "factories.cpp"
    "player_action.cpp"
    "system.cpp"
)
    
add_library(
    game_system
    ${GAMESYS_SRC}
)

target_link_libraries(
game_system 
spdlog::spdlog
game_component
pthread
tbb
fmt
)
