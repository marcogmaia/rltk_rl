include_directories(
    # "include"
    "."
    # "./component"
    "./system"
)

# add_subdirectory("./component")
add_subdirectory("./utils")
find_package(spdlog REQUIRED)

add_library(
    game
    "game.cpp"
    "engine.cpp"
    "map.cpp"
    "system/move_attack.cpp"
    "system/player_action.cpp"
)

# target_link_libraries(game entity pthread fmt LIBTCOD)
target_link_libraries(game PRIVATE spdlog::spdlog)
target_link_libraries(game PUBLIC pthread  tbb fmt LIBTCOD)

# set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_BUILD_TYPE DEBUG)
