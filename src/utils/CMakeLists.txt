
set(
    UTILS_SRC
    "fov.c"
    "utils.cpp"
    "rng.cpp"
   
)

add_library(
    game_utils
    ${UTILS_SRC}
)

target_link_libraries(game_utils PUBLIC fmt rltk)

