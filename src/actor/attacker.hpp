#pragma once

class Actor;

class Attacker {
private:
public:
    float power = 0.f;

    // Attacker() = default;
    Attacker(float power);
    void attack(Actor* owner, Actor* target);
};

class AttackerPlayer : public Attacker {};  // ? idk if I'll use this1