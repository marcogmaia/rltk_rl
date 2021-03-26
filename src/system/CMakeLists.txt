# include_directories(
#     "."
#     "include"
#     "entity"
#     "component"
#     "system"
# )


# add_library(
#     game
#     "game.cpp"
#     "engine.cpp"
#     "map.cpp"
# )

# # target_link_libraries(game entity pthread fmt LIBTCOD)
# target_link_libraries(game pthread tbb fmt LIBTCOD)

# # set(CMAKE_BUILD_TYPE RELEASE)
# set(CMAKE_BUILD_TYPE DEBUG)
