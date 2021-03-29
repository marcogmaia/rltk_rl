find_package(spdlog REQUIRED)

include_directories(
    "."
)


add_subdirectory("./utils")
add_subdirectory("./system")
add_subdirectory("./component")

add_library(
    game
    "game.cpp"
    "engine.cpp"
    "map.cpp"
    "factories.cpp"
)

target_link_libraries(
    game PRIVATE
    radl_utils
    game_system
    game_component
)
    
target_link_libraries(
    game PUBLIC
    spdlog::spdlog
    pthread
    tbb
    fmt
    rltk
)

# set(CMAKE_BUILD_TYPE RELEASE)
# set(CMAKE_BUILD_TYPE DEBUG)
