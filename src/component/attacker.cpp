#include "core/engine.hpp"
#include "attacker.hpp"

namespace radl {

namespace {

using namespace world;
using engine::reg;

void die(entity& ent) {
    reg.remove<being_t>(ent);
    reg.remove<blocks_t>(ent);
    reg.replace<renderable_t>(ent, renderable_t{.vchar = vchar_t{
                                                    '%',
                                                    RED,
                                                    DARKEST_RED,
                                                }});
}

}  // namespace

namespace {
std::mutex attack_mutex;
}

void attack(const entity& ent_atk, const position_t& pos) {
    std::lock_guard lock(attack_mutex);
    auto& ents = engine::get_map().at(pos).entities_here;
    auto found = std::ranges::find_if(ents, [&](auto& ent) {
        return reg.all_of<being_t, destructible_t>(ent);
    });
    if(found == ents.end()) {
        return;
    }
    // found
    auto& ent_def = *found;

    auto& attacker    = reg.get<attacker_t>(ent_atk);
    auto& defender    = reg.get<destructible_t>(ent_def);
    auto final_damage = attacker.power - defender.defense;
    if(final_damage < 0) {
        final_damage = 0;
    }

    if(!defender.is_dead()) {
        // FIXME: beign_t is gone, I'm calling this with a dead entity
        spdlog::debug("The {} attacks the {} for {} damage.",
                      reg.get<being_t>(ent_atk).name,
                      reg.get<being_t>(ent_def).name, final_damage);
        reg.patch<destructible_t>(ent_def, [&](destructible_t& def) {
            def.hp -= final_damage;
        });
        // if dead die... nice
        if(defender.is_dead()) {
            die(ent_def);
        }
    }
}

}  // namespace radl
