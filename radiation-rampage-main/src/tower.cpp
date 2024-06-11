#include <iostream>
#include <vector>
#include "tower.h"
#include "barracks.h"
#include "bombTower.h"
#include "laserTower.h"
#include "towerGUI.h"
#include "constants.h"
#include "initAudio.h"

Tower::Tower(const std::string& name, int health, int damage, int range, DamageType damageType, const TowerLocation& location, int buildCost, double fireRate)
    : name(name), health(health), damage(damage), range(range), damageType(damageType), location(location), buildCost(buildCost), fireRate(fireRate), timeSinceLastSlowDegrade(0.0) {}

std::shared_ptr<Tower> Tower::createTower(const std::string& type, const TowerLocation& location, View* view) {
    using namespace tower;
    playSound("../resource/build.wav");
    if (type == "Barracks") {
        return std::make_shared<Barracks>("Barracks", tower::barracksHealth, tower::barracksDamage, tower::barracksRange, 
                            tower::barracksDamageType, location, tower::barracksBuildCost, tower::barracksFireRate, view);
    } else if (type == "Bomb") {
        return std::make_shared<BombTower>("BombTower", tower::bombHealth, tower::bombDamage, tower::bombRange, 
                            tower::bombDamageType, location, tower::bombBuildCost, tower::bombFireRate, tower::bombRangeBombEffect, view);
    } else if (type == "Laser") {
        return std::make_shared<LaserTower>("LaserTower", tower::laserHealth, tower::laserDamage, tower::laserRange,
                            tower::laserDamageType, location, tower::laserBuildCost, tower::laserFireRate, view);
    }
    return nullptr;
}

void Tower::updateTarget(std::vector<Enemy>& enemies) {}

bool Tower::isInRange(int x, int y) const {
    int towerX = location.x + location.size / 2;
    int towerY = location.y + location.size / 2;
    int distanceSquared = (x - towerX) * (x - towerX) + (y - towerY) * (y - towerY);
    int rangeSquared = range * range;
    return distanceSquared <= rangeSquared;
}

void Tower::repair() {
    using namespace tower;
    if (name == "Barracks") {
        setHealth(tower::barracksHealth);
    } else if (name == "BombTower") {
        setHealth(tower::bombHealth);
    } else if (name == "LaserTower") {
        setHealth(tower::laserHealth);
    }
}

int Tower::getRepairCost() const {
    if (health == 0) {
        return buildCost;
    }

    float healthPercent = static_cast<float>(health) / 100.0f;
    if (healthPercent >= 0.5f && healthPercent < 1.0f) {
        // Tower health 50% - 99%
        return static_cast<int>(buildCost * 0.4f);
    } else if (healthPercent > 0.0f && healthPercent < 0.5f) {
        // Tower health 1% - 49%
        return static_cast<int>(buildCost * 0.6f);
    }
    // Tower health is already 100%, no need to repair
    return 0;
}

int Tower::getSellEarnings() const {
    float healthPercent = static_cast<float>(health) / 100.0f;
    if (healthPercent >= 0.5f && healthPercent <= 1.0f) {
        // Tower health 50% - 100% 
        return static_cast<int>(buildCost * 0.6f);
    } else {
        // Tower health 0% - 49%
        return static_cast<int>(buildCost * 0.4f);
    }
}

bool Tower::isReadyToSlowDegrade(double elapsedTime){
    using namespace tower;
    timeSinceLastSlowDegrade += elapsedTime;
    if (timeSinceLastSlowDegrade < tower::secondsBetweenSlowDegrade){
        return false;
    }
    // Enough time has passed. Can slow degrade now
    timeSinceLastSlowDegrade = 0.0;
    return true;
}


void Tower::slowDegrade(int mod){
    using namespace tower;
    takeDamage((tower::slowDegradeAmount * mod));
}

bool Tower::isDestroyed(){
    if (health == 0){
        return true;
    }else{
        return false;
    }
}

void Tower::takeDamage(int damageAmount){
    // Tower health cannot be negative 
    if (health - damageAmount >= 0){
        health-= damageAmount;
    }else if (health - damageAmount < 0 ){
        // if the damage is more than tower health set health to 0
        health = 0;
    }
}
