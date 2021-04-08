
#include "component/component.hpp"
#include "core/engine.hpp"

namespace radl {

namespace {

using engine::reg;
std::mutex attack_mutex;

}  // namespace

void attack(const entity& ent_atk, const entity& ent_def) {
    static std::mutex melee_mutex;
    std::lock_guard lock(melee_mutex);
    if(!reg.any_of<dead_t>(ent_def)) {
        reg.emplace_or_replace<wants_to_melee_t>(ent_atk, ent_def);
    }
}

void attack(const entity& ent_atk, const position_t& pos) {
    std::lock_guard lock(attack_mutex);

    auto& ents = engine::get_map().at(pos).entities_here;

    auto view_attackable
        = reg.view<being_t, combat_stats_t>(entt::exclude<dead_t>);

    auto found = std::ranges::find_if(ents, [&](auto& ent) {
        return view_attackable.contains(ent);
    });

    auto not_found = found == ents.end();
    if(not_found) {
        return;
    }
    // found
    auto& ent_def = *found;
    attack(ent_atk, ent_def);
}

}  // namespace radl