#ifndef BOMB_TOWER_H
#define BOMB_TOWER_H

#include "tower.h"
#include "constants.h"

class BombTower : public Tower {
private:
    View* view;
    int rangeBombEffect;
    double timeSinceLastAttack;
    bool inBombRange(int targetX, int targetY, int otherEnemyX, int otherEnemyY) const;

public:
    BombTower(std::string name, int health, int damage, int range, DamageType damageType, const TowerLocation& location, int buildCost, int fireRate, int rangeBombEffect, View* view);

    void attack() override;

    void updateTarget(std::vector<Enemy>& enemies) override;

    bool isReadyToAttack(double elapsedTime);

    void upgrade() override;

    int getUpgradeCost() const override;
};

#endif