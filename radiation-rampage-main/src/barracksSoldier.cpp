#include "barracksSoldier.h"
#include "constants.h"

Soldier::Soldier(int health, int x, int y)
    : health(health), x(x), y(y) {}

void Soldier::takeDamage(int damage) {
    health -= damage;
}

bool Soldier::isAlive() const {
    return health > 0;
}

void Soldier::setTimeSinceDeath(double time) {
    timeSinceDeath = time;
}

void Soldier::respawn() {
    using namespace tower;
    timeSinceDeath = 0.0;
    health = tower::barracksSoldierHealth;
}