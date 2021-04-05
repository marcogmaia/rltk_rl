#pragma once
#include <string>

namespace radl {


struct destructible_t {
    float max_hp;             // maximum health points
    float hp;                 // current health points
    float defense;            // hit points deflected
    std::string corpse_name;  // the entity's name once dead/destroyed
 
    [[nodiscard]] inline bool is_dead() const {
        return hp <= 0;
    }

};


}  // namespace radl
