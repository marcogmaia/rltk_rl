include_directories(
    "."
)

set(
    UTILS_SRC
    "utils.cpp"
    "rng.cpp"
)

add_library(
    radl_utils
    ${UTILS_SRC}
)

target_link_libraries(radl_utils PUBLIC fmt rltk)

