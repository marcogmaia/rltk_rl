#pragma once

class Actor;

class Destructible {
public:
    float maxHp;              // maximum health points
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
    DestructiblePlayer(float max_hp, float defense, const char* corpse_name);
};

class DestructibleObject : public Destructible {
    DestructibleObject(float max_hp, float defense, const char* corpse_name);
};
