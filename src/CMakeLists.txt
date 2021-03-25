



include_directories(
    "."
    "include"
    "entity"
)

add_subdirectory("./entity")

add_library(
    game
    "game.cpp"
    "engine.cpp"
    "map.cpp"
    "gui.cpp"
)

target_link_libraries(game entity pthread fmt LIBTCOD)

# set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_BUILD_TYPE DEBUG)

