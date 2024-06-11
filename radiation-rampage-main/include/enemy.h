#ifndef ENEMY_H
#define ENEMY_H

#include <string>
#include <memory>
#include "weather.h"

enum class Direction {
	NORTH,
	EAST,
	SOUTH,
	WEST
};

class Enemy {
private:
    std::string name;
    int health;
    int speed;
    int x; // center x coordinate of the enemy
    int y; // center y coordinate of the enemy
    Direction dir;
    int damage; 
    bool flying;
    int coins;

    int distanceTraveled;
    bool halted = false;

    //weather stored for helping getting modifiers
    std::shared_ptr<Weather> weather;

    void pathCornerCollision(); // updates enemy direction when an enemy collides with a path corner 
    bool inPathCornerRange(int cornerX1, int cornerY1, int cornerX2, int cornerY2); // helper function for pathCornerCollision(), checks if enemy is in range of a path corner

public:
	// Constructor
    Enemy(std::string name, int health, int speed, int x, int y, Direction direct, int damage, bool flying, int coin, std::shared_ptr<Weather> weather);

    // Enemy takes damage
    void takeDamage(int damage);
    
    // Checks if health is more than 0
    bool isAlive() const;
    
    // Moves the enemy
    void move();

    // Halt and resume enemy movement
    void haltMovement() { halted = true; }
    void resumeMovement() { halted = false; }
    
    // Getters
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getSpeed() const { return speed; }
    int getX() const { return x; }
    int getY() const { return y; }
    Direction getDirection() const { return dir; }
    int getDamage() const { return damage; }
    int getDistanceTraveled() const { return distanceTraveled; }
    int getCoins() const { return coins; }
    bool getFlying() const { return flying; }

    // Setters
    void setName(std::string n) { name = n; }
    void setHealth(int h) { health = h; }
    void setSpeed(int s) { speed = s; }
    void setX(int x1) { x = x1; }
    void setY(int y1) { y = y1; }
    void setDirection(Direction d) { dir = d; }
    void setDamage(int d) { damage = d; }
};

#endif