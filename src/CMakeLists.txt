
include_directories("include")
include_directories(".")

add_library(
    game
   "game.cpp"
   "engine.cpp"
   "map.cpp"
   "actor.cpp"
)

target_link_libraries(game LIBTCOD)

# set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_BUILD_TYPE DEBUG)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
