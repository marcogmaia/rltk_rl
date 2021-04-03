
include_directories(
    "."
)

set(COMPONENT_SCRS
    "attacker.cpp"
    "viewshed.cpp"
    "attacker.cpp"
    "destructible.cpp"
    "viewshed.cpp"
    "ai.cpp"
)

add_library(
    game_component
    ${COMPONENT_SCRS}
)



target_link_libraries(game_component tbb spdlog::spdlog)
