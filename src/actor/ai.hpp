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
    /**
     * @brief updates the owners state
     *
     * @param owner
     * @return true if updated, false otherwise
     */
    bool update(Actor* owner) override;
};

class AiEnemy : public Ai {
    /**
     * @brief move self or attack other actor
     *
     * @param pos position to move or attack
     * @return true if moved, false otherwise
     */
    bool move_attack(Actor* owner, position_t pos);

public:
    AiEnemy() = default;
    bool update(Actor* owner) override;
};
