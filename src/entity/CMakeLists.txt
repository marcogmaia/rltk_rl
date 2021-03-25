
include_directories(".")

set(
    ENTITY_SRCS
    "ai.cpp"
    "entity.cpp"
    "destructible.cpp"
    "attacker.cpp"
)

add_library(
    entity
    "${ENTITY_SRCS}"
)
    
target_link_libraries(entity LIBTCOD)
