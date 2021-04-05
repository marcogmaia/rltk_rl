
set(
    GAMESYS_SRC
    "camera.cpp"
    "player_action.cpp"
    "combat.cpp"
    # "move_attack.cpp"
    # "visibility.cpp"
   
)
    
# find_package(spdlog REQUIRED)

add_library(
    game_system
    ${GAMESYS_SRC}
)

target_link_libraries(game_system game_component spdlog::spdlog pthread tbb fmt)

set(CMAKE_BUILD_TYPE DEBUG)
