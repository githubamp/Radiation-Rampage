#include <memory>
#include "bombTower.h"
#include "initAudio.h"

BombTower::BombTower(std::string name, int health, int damage, int range, DamageType damageType, const TowerLocation& location, int buildCost, int fireRate, int rangeBombEffect, View* view)
    : Tower(name, health, damage, range, damageType, location, buildCost, fireRate), rangeBombEffect(rangeBombEffect), view(view), timeSinceLastAttack(0.0){
}

void BombTower::attack() {
    // Bomb attacks in update target
}

int BombTower::getUpgradeCost() const {
    using namespace tower;
    return (int)(tower::bombBuildCost * tower::bombUpgradeCostMod);
}

void BombTower::upgrade() {
    using namespace tower;
    setUpgraded(true);
    setDamage(tower::bombUpgradeDamage);
    setFireRate(tower::bombUpgradeFireRate);
}

void BombTower::updateTarget(std::vector<Enemy>& enemies){
    // Find the target with the most distance traveled
    int maxDistance = -1;
    std::shared_ptr<Enemy> target;
    for (auto& enemy : enemies) {
        if (isInRange(enemy.getX(), enemy.getY()) && enemy.isAlive() && enemy.getFlying() == false) {
            // Bomb can only target alive and NON-flying enemies
            int distance = enemy.getDistanceTraveled();
            if (distance > maxDistance) {
                target = std::make_shared<Enemy>(enemy);
                maxDistance = distance;
            }
        }
    }

    // If target found, launch bomb and affect other enemies too
    if (target){

        // Render bomb visual 
        const TowerLocation& towerLocation = getLocation();
        int centerX = towerLocation.x + towerLocation.size / 2;
        int centerY = towerLocation.y + towerLocation.size / 2;
        view->triggerAttackAnimation(centerX, centerY, target->getX(), target->getY(), DamageType::BOMB); 
        for (auto& enemy : enemies){
            if(inBombRange(target->getX(), target->getY(), enemy.getX(), enemy.getY())){
                if (enemy.isAlive() && enemy.getFlying() == false){
                    // Bomb can only hit enemies that are alive and NON-flying enemies
                    enemy.takeDamage(getDamage());
                }
            }
        }
        //play bomb sound
        playSound("../resource/Bomb.wav"); 
    }
}

bool BombTower::isReadyToAttack(double elapsedTime){
    // Increment the time since the last attack
    timeSinceLastAttack += elapsedTime;

    if (timeSinceLastAttack >= 3.0 / getFireRate() ){
        timeSinceLastAttack = 0.0;
        return true;
    }
    return false;
}

bool BombTower::inBombRange(int targetX, int targetY, int otherEnemyX, int otherEnemyY) const{
    int distanceSquared = (otherEnemyX - targetX) * (otherEnemyX - targetX) + (otherEnemyY - targetY) * (otherEnemyY - targetY);
    int rangeSquared = rangeBombEffect * rangeBombEffect;
    return distanceSquared <= rangeSquared;
}

