#include "position.hpp"
#include "move.hpp"
#include "attacker.hpp"

namespace radl {

bool move(entt::registry& reg, entt::entity& e, const position_t& dpos) {
    auto has_attacker = reg.has<position_t>(e);
    if(has_attacker) {
        auto o = reg.get<position_t>(e);
        o += dpos;
    }
}

}  // namespace radl