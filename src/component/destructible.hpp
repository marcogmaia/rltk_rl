#pragma once
#include <string>

// class Entity;

struct destructible_t {
    float max_hp;             // maximum health points
    float hp;                 // current health points
    float defense;            // hit points deflected
    std::string corpse_name;  // the entity's name once dead/destroyed

    // destructible_t(float max_hp, float defense, const char* corpse_name);
    // virtual ~destructible_t() = default;
};

inline bool is_dead(destructible_t& dstb) {
    return dstb.hp <= 0;
}
// class DestructiblePlayer final : public destructible_t {
// public:
//     DestructiblePlayer(float max_hp, float defense, const char* corpse_name);
//     void die(Entity* owner) override;
// };

// class DestructibleEnemy final : public destructible_t {
// public:
//     DestructibleEnemy(float max_hp, float defense, const char* corpse_name);
//     void die(Entity* owner) override;
// };
