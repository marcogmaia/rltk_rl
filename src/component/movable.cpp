
#include "component/movable.hpp"

namespace radl {
namespace {

std::mutex walk_mutex;
using engine::reg;
}  // namespace

void walk(const entt::entity& ent, const position_t& src_pos,
          const position_t& target_pos) {
    // if(!engine::get_map().at(target_pos).characteristics.walkable) {
    //     return;
    // }
    // std::lock_guard guard(walk_mutex);
    // if(!world::is_occupied(reg, target_pos) && (src_pos != target_pos)) {
    //     world::map_entity_walk(ent, src_pos, target_pos);
    //     // mark viewshed as dirty to recalculate
    //     reg.get<world::viewshed_t>(ent).dirty = true;
    // }


    reg.emplace<want_to_walk_t>(ent, want_to_walk_t{
                                         .from = src_pos,
                                         .to   = target_pos,
                                     });

    // auto ents_movable = reg.view<being_t, position_t, want_to_walk_t>();
    // std::for_each(ents_movable.begin(), ents_movable.end(),
    //               [&](auto ent, being_t& being, want_to_walk_t& want_walk) {
    //                   want_walk.from = src_pos;
    //                   want_walk.to   = target_pos;
    //               });
}
}  // namespace radl
