set(COMPONENT_SCRS
    "ai.cpp"
    "combat.cpp"
    "movable.cpp"
    "viewshed.cpp"
)

add_library(
    game_component
    ${COMPONENT_SCRS}
)

target_link_libraries(game_component PUBLIC tbb spdlog::spdlog)
