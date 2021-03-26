include_directories(
    # "include"
    "."
    "./component"
    "./system"
)

# add_subdirectory("./component")
# add_subdirectory("./system")

add_library(
    game
    "game.cpp"
    "engine.cpp"
    # "map.cpp"
    "system/move.cpp"
    "system/player_action.cpp"
)

# target_link_libraries(game entity pthread fmt LIBTCOD)
target_link_libraries(game pthread tbb fmt LIBTCOD)

# set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_BUILD_TYPE DEBUG)
