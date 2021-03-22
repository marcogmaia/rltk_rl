#pragma once
#include "common.hpp"

class Actor;

class Ai {
public:
    virtual bool update(Actor* owner) = 0;
};

class AiPlayer : public Ai {
private:
    /**
     * @brief move self or attack other actor
     *
     * @param pos position to move or attack
     * @return true if moved, false otherwise
     */
    bool move_attack(Actor* owner, position_t pos);

public:
    AiPlayer() = default;
    bool update(Actor* owner) override;
};
