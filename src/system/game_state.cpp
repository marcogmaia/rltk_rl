#include "spdlog/spdlog.h"
#include "system/game_state.hpp"
#include "system/map/dijkstra_map.hpp"
#include "system/log.hpp"
#include "system/factories.hpp"


namespace radl {

registry reg;
entity player;

void game_state_init() {
    reg.set<game_log_t>();
    auto& map = reg.ctx<Map>();
    reg.set<DijkstraMap>(map);
}

}  // namespace radl
