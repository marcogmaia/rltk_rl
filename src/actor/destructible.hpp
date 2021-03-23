#pragma once
#include <string>

class Actor;

class Destructible {
public:
    float max_hp;             // maximum health points
    float hp;                 // current health points
    float defense;            // hit points deflected
    const char* corpse_name;  // the actor's name once dead/destroyed

    Destructible(float max_hp, float defense, const char* corpse_name);

    inline bool is_dead() {
        return hp <= 0;
    }

    /**
     * @brief
     *
     * @param owner actor
     * @param damage to process
     * @return returns the damage taken
     */
    float take_damage(Actor* owner, float damage);

    virtual void die(Actor* owner);
};

class DestructiblePlayer : public Destructible {
public:
    DestructiblePlayer(float max_hp, float defense, const char* corpse_name);
    void die(Actor* owner) override;
};

class DestructibleEnemy : public Destructible {
public:
    DestructibleEnemy(float max_hp, float defense, const char* corpse_name);
    void die(Actor* owner) override;
};
