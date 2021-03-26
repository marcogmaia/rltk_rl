
include_directories(
    "."
    "../src"
    "../src/entity"
    "../src/include"
)

# add_subdirectory("../src")

add_executable(
    map_view
    "main.cpp"
)

target_link_libraries(map_view game tbb pthread fmt LIBTCOD)

# set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_BUILD_TYPE DEBUG)

