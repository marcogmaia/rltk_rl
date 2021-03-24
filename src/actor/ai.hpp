#pragma once
#include "common.hpp"

class Actor;

class Ai {
public:
    virtual bool update(Actor* owner) = 0;
};

class AiPlayer : public Ai {
protected:
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
protected:
    /**
     * @brief move self or attack other actor
     *
     * @param pos position to move or attack
     * @return true if moved, false otherwise
     */
    virtual bool move_attack(Actor* owner, position_t pos);

    // how many turns the monster chases the player
    // after losing his sight
    static const int TRACKING_TURNS = 3;
    int move_count                  = TRACKING_TURNS;

public:
    AiEnemy() = default;
    bool update(Actor* owner) override;
};
