#include "spdlog/spdlog.h"
#include "game_state.hpp"
#include "core/map/dijkstra_map.hpp"
#include "core/log.hpp"
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
