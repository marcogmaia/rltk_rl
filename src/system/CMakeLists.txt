
set(
    GAMESYS_SRC
    "camera.cpp"
    "player_action.cpp"
    "system.cpp"
    # "move_attack.cpp"
    # "visibility.cpp"
   
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

set(CMAKE_BUILD_TYPE DEBUG)
