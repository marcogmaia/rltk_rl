#pragma once

#include "entt/entt.hpp"
#include "attacker.hpp"
#include "destructible.hpp"
#include "entity.hpp"

entt::entity make_enemy(entt::registry& reg) {
    entt::entity e = reg.create();
    reg.emplace<Entity>(e, )

        return e;
}