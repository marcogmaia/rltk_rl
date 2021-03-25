#pragma once
#include "common.hpp"

class Entity;

class Ai {
public:
    virtual bool update(Entity* owner) = 0;
    virtual ~Ai() = default;
};

class AiPlayer final : public Ai {
protected:
    /**
     * @brief move self or attack other entity
     *
     * @param pos position to move or attack
     * @return true if moved, false otherwise
     */
    bool move_attack(Entity* owner, position_t pos);

public:
    AiPlayer() = default;
    /**
     * @brief updates the owners state
     *
     * @param owner
     * @return true if updated, false otherwise
     */
    bool update(Entity* owner) override;
};

class AiEnemy final : public Ai {
protected:
    /**
     * @brief move self or attack other entity
     *
     * @param pos position to move or attack
     * @return true if moved, false otherwise
     */
    virtual bool move_attack(Entity* owner, position_t pos);

    // how many turns the monster chases the player
    // after losing his sight
    static const int TRACKING_TURNS = 3;
    int move_count                  = TRACKING_TURNS;

public:
    AiEnemy() = default;
    bool update(Entity* owner) override;
};
