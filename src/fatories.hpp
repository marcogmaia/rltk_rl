#pragma once

#include "entt/entt.hpp"
#include "attacker.hpp"
#include "destructible.hpp"

namespace radl {

entt::entity make_enemy(entt::registry& reg, const entt::entity& ent) {
    entt::entity e = reg.create();

    // TODO factories
    reg.emplace<>(ent)

        return ent;
}

}  // namespace radl