
#include <mutex>

#include "component/combat.hpp"
#include "component/movable.hpp"

namespace radl::component {

void walk(registry& reg, const entt::entity& ent, const position_t& src_pos,
          const position_t& target_pos) {
    static std::mutex walk_mutex;
    std::lock_guard lock(walk_mutex);

    if(!reg.any_of<dead_t>(ent)) {
        reg.emplace<want_to_walk_t>(ent, want_to_walk_t{
                                             .from = src_pos,
                                             .to   = target_pos,
                                         });
    }
}

}  // namespace radl::component
