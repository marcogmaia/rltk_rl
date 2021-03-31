include_directories(
    "."
)
set(
    GAMESYS_SRC
    "move_attack.cpp"
    # "visibility.cpp"
    "player_action.cpp"
    "camera.cpp"
    )
    
    find_package(spdlog REQUIRED)
add_library(
    game_system
    ${GAMESYS_SRC}
)

target_link_libraries(game_system spdlog::spdlog pthread tbb fmt)

set(CMAKE_BUILD_TYPE DEBUG)
