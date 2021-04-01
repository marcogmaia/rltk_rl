#pragma once
#include "core/engine.hpp"
#include "spdlog/spdlog.h"

namespace radl {

struct ai_t {
    // void update() = 0;
};

// struct ai_enemy_t : public ai_t {
//     inline void update() override {

//         spdlog::debug("enemy_!! ai");
//     }
// };

void ai_sys(entt::registry &reg, entt::entity ent);


}  // namespace radl
