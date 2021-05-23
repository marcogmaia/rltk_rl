
set(
    UTILS_SRC
    "fov.c"
    "permissive-fov/permissive-fov.cpp"
    "rng.cpp"
    "utils.cpp"
   
)

add_library(
    game_utils
    ${UTILS_SRC}
)

target_link_libraries(game_utils PUBLIC fmt rltk)

