#pragma once
#include <string>

// class Entity;

struct Destructible {
    float max_hp;             // maximum health points
    float hp;                 // current health points
    float defense;            // hit points deflected
    std::string corpse_name;  // the entity's name once dead/destroyed

    // Destructible(float max_hp, float defense, const char* corpse_name);
    // virtual ~Destructible() = default;
};

inline bool is_dead(Destructible& dstb) {
    return dstb.hp <= 0;
}
// class DestructiblePlayer final : public Destructible {
// public:
//     DestructiblePlayer(float max_hp, float defense, const char* corpse_name);
//     void die(Entity* owner) override;
// };

// class DestructibleEnemy final : public Destructible {
// public:
//     DestructibleEnemy(float max_hp, float defense, const char* corpse_name);
//     void die(Entity* owner) override;
// };
