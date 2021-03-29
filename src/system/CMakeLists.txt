include_directories(
    "."
)

set(
    GAMESYS_SRC
    "move_attack.cpp"
    "visibility.cpp"
    "player_action.cpp"
)

add_library(
    game_system
    ${GAMESYS_SRC}
)

target_link_libraries(game_system pthread tbb fmt)

set(CMAKE_BUILD_TYPE DEBUG)
