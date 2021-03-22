



include_directories(
    "."
    "include"
    "actor"
)

add_subdirectory("./actor")

add_library(
    game
    "game.cpp"
    "engine.cpp"
    "map.cpp"
)

target_link_libraries(game actor pthread fmt LIBTCOD)

# set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_BUILD_TYPE DEBUG)

