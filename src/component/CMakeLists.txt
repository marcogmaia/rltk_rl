set(COMPONENT_SCRS
    "ai.cpp"
    "combat.cpp"
    "inventory.cpp"
    "movable.cpp"
    "particle.cpp"
    "renderable.cpp"
    "viewshed.cpp"
)

add_library(
    game_component
    ${COMPONENT_SCRS}
)

target_link_libraries(game_component PUBLIC tbb spdlog::spdlog)
