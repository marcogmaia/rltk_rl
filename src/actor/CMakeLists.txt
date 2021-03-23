
include_directories(".")

set(
    ACTOR_SRCS
    "ai.cpp"
    "actor.cpp"
    "destructible.cpp"
    "attacker.cpp"
)

add_library(
    actor
    "${ACTOR_SRCS}"
)
    
target_link_libraries(actor LIBTCOD)
