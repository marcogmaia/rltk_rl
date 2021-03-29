include_directories(
    "."
)

add_subdirectory("./utils")
add_subdirectory("./system")
add_subdirectory("./component")
find_package(spdlog REQUIRED)

add_library(
    game
    "game.cpp"
    "engine.cpp"
    "map.cpp"
)

target_link_libraries(
    game PRIVATE
    radl_utils
    game_system
    game_component
    spdlog::spdlog
)

target_link_libraries(
    game PUBLIC
    pthread
    tbb
    fmt
    rltk
)

# set(CMAKE_BUILD_TYPE RELEASE)
# set(CMAKE_BUILD_TYPE DEBUG)
