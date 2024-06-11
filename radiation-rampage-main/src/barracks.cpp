#include <limits>
#include "barracks.h"
#include "constants.h"
#include "initAudio.h"

std::vector<Soldier> Barracks::allSoldiers;

Barracks::Barracks(std::string name, int health, int damage, int range, DamageType damageType, const TowerLocation& location, int buildCost, double fireRate, View* view)
    : Tower(name, health, damage, range, damageType, location, buildCost, fireRate), view(view), target(nullptr), elapsedTime(0.0), timeSinceLastAttack(0.0), soldierLocations({
        {115, 109},
        {115, 441},
        {595, 544},
        {900, 155},
        {1060, 290}
    }) {
    using namespace tower;
    // Initialize soldiers
    if (allSoldiers.empty()) {
        for (const auto& location : soldierLocations) {
            allSoldiers.emplace_back(tower::barracksSoldierHealth, location.first, location.second);
        }
    }
}

void Barracks::attack() {
    // Check if the tower has a target and the target is alive
    if (target && target->isAlive()) {
        // Get the soldier associated with this barracks tower
        std::pair<int, int> soldierLocation = getTowerSoldierMapping();
        Soldier& soldier = getSoldierAtLocation(soldierLocation);
        
        if (soldier.isAlive() && !isDestroyed()) {
            target->haltMovement();
            if (isReadyToAttack(elapsedTime)) {
                target->takeDamage(getDamage());
                soldier.takeDamage(target->getDamage());
                playSound("../resource/Barracks.wav");
            }
        }

        if (!soldier.isAlive() || isDestroyed()) {
            target->resumeMovement();
        }
    }
// Render alive soldiers
view->renderSoldiers();
}

// Calculate distance between soldier and enemy
double Barracks::calculateDistance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Update target enemy to the closest enemy within range of a soldier
void Barracks::updateTarget(std::vector<Enemy>& enemies) {
    double minDistance = std::numeric_limits<double>::max();
    target = nullptr;

    // Find the closest enemy within range
    for (auto& soldier : allSoldiers) {
        if (target) {
            break;
        }
        for (auto& enemy : enemies) {
            if (isEnemyNearSoldier(enemy) && !enemy.getFlying()) {
                double distance = calculateDistance(enemy.getX(), enemy.getY(), soldier.getX(), soldier.getY());
                
                if (distance < minDistance) {
                    minDistance = distance;
                    target = &enemy;
                    break;
                }
            }
        }
    }
}

// Used to resume enemy movement if enemy is halted while tower is sold
void Barracks::resumeEnemyMovement() {
    if (target && target->isAlive()) {
        target->resumeMovement();
    }
}

// Determine if an enemy is within range of a soldier
bool Barracks::isEnemyNearSoldier(const Enemy& enemy) {
    // Get the soldier location associated with a barracks tower
    std::pair<int, int> soldierLocation = getTowerSoldierMapping();

    // Check if the enemy is North or East of the soldier
    if ((enemy.getX() >= soldierLocation.first - 30) &&
        (enemy.getX() <= soldierLocation.first + 80) &&
        (enemy.getY() >= soldierLocation.second - 30) &&
        (enemy.getY() <= soldierLocation.second + 80)) {
        return true;
    }
    return false;
}

bool Barracks::isReadyToAttack(double elapsedTime) {
    // Increment the time since the last attack
    timeSinceLastAttack += elapsedTime;

    // Check if enough time has elapsed to perform another attack
    if (timeSinceLastAttack >= 1.0 / getFireRate()) {
        timeSinceLastAttack = 0.0;
        return true;
    }

    return false;
}

// Return the soldier location that corresponds to the tower location
std::pair<int, int> Barracks::getTowerSoldierMapping() const {
    const TowerLocation& thisTowerLocation = this->getLocation();

    // Find the index of this barracks tower location
    int index = -1;
    for (size_t i = 0; i < TowerLocationManager::getTowerLocations().size(); ++i) {
        if (TowerLocationManager::getTowerLocations()[i] == thisTowerLocation) {
            index = static_cast<int>(i);
            break;
        }
    }

    // Return the corresponding soldier location
    if (index != -1) {
        if (index < static_cast<int>(soldierLocations.size())) {
            return soldierLocations[index];
        }
    }
    return std::make_pair(-1, -1);
}

// Return the soldier that corresponds to the soldier location
Soldier& Barracks::getSoldierAtLocation(const std::pair<int, int>& location) {
    for (auto& soldier : allSoldiers) {
        if (soldier.getX() == location.first && soldier.getY() == location.second) {
            return soldier;
        }
    }
    throw std::runtime_error("No soldier found at the specified location.");
}

// Respawn soldier after 8 seconds of death
void Barracks::handleSoldierRespawnTiming(double elapsedTime) {
    // Get the soldier associated with this barracks tower
    std::pair<int, int> soldierLocation = getTowerSoldierMapping();
    Soldier& soldier = getSoldierAtLocation(soldierLocation);

    if (!soldier.isAlive()) {
        double timeSinceDeath = soldier.getTimeSinceDeath();
        timeSinceDeath += elapsedTime;
        soldier.setTimeSinceDeath(timeSinceDeath);

        if (timeSinceDeath >= 5.0) {
            soldier.respawn();
            playSound("../resource/BarracksSpawn.wav");
        }

    } else {
        soldier.setTimeSinceDeath(0.0);
    }
}

int Barracks::getUpgradeCost() const{
    using namespace tower;
    return (int)(tower::barracksBuildCost * tower::barracksUpgradeCostMod);
}

void Barracks::upgrade() {
    using namespace tower;
    setUpgraded(true);
    setDamage(tower::barracksUpgradeDamage);
    setFireRate(tower::barracksUpgradeFireRate);
}

void Barracks::update(double elapsedTime) {
    this->elapsedTime = elapsedTime;
    handleSoldierRespawnTiming(elapsedTime);
    attack();
}