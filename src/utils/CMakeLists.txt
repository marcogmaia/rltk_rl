include_directories(
    "."
)

set(
    UTILS_SRC
    "utils.cpp"
)

add_library(
    radl_utils
    ${UTILS_SRC}
)

# target_link_libraries(game entity pthread fmt LIBTCOD)
target_link_libraries(radl_utils PUBLIC fmt LIBTCOD)

# set(CMAKE_BUILD_TYPE RELEASE)
# set(CMAKE_BUILD_TYPE DEBUG)
