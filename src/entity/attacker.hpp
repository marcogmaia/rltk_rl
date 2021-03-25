#pragma once

class Entity;

class Attacker {
private:
public:
    float power = 0.F;

    // Attacker() = default;
    explicit Attacker(float power);
    void attack(Entity* owner, Entity* target);
};

class AttackerPlayer : public Attacker {};  // ? idk if I'll use this1