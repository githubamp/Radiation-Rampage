#include <iostream>
#include <algorithm>
#include <memory>
#include "enemy.h" 
#include "constants.h" 
#include "weather.h"

const int UPDATE_DIVISOR = 2;

Enemy::Enemy(std::string n, int h, int s, int x1, int y1, Direction direct, int d, bool flying, int c, std::shared_ptr<Weather> w)
    : name(n), health(h), speed(s), x(x1), y(y1), dir(direct), damage(d), flying(flying), coins(c), weather(w){}

void Enemy::takeDamage(int d){
	health -= d / weather->getenemyHpMod();
}

bool Enemy::isAlive() const{
	return health > 0;
}

void Enemy::move() {
	if (!halted) {
		pathCornerCollision();
		int disToMov = speed + weather->getenemySpeedMod();

		if (dir == Direction::EAST) {
			x += disToMov;
		} else if (dir == Direction::WEST) {
			x -= disToMov;
		} else if (dir == Direction::NORTH) {
			y -= disToMov;
		} else if (dir == Direction::SOUTH) {
			y += disToMov;
		}

        distanceTraveled += std::abs(disToMov);
	}
}

void Enemy::pathCornerCollision(){
	using namespace enemy;
	if (inPathCornerRange(enemy::corner1X1, enemy::corner1Y1, enemy::corner1X2, enemy::corner1Y2)){
		// checks if the enemy is above the diagonal line
		if (y > ((enemy::corner1Slope*x) + enemy::corner1YIntercept)){
			dir = Direction::EAST;
		}
	}else if (inPathCornerRange(enemy::corner2X1, enemy::corner2Y1, enemy::corner2X2, enemy::corner2Y2)){
		// checks if the enemy is below the diagonal line
		if (y < ((enemy::corner2Slope *x) + enemy::corner2YIntercept)){
			dir = Direction::NORTH;
		}
	}else if (inPathCornerRange(enemy::corner3X1, enemy::corner3Y1, enemy::corner3X2, enemy::corner3Y2)){
		// checks if the enemy is below the diagonal line
		if (y < ((enemy::corner3Slope *x) + enemy::corner3YIntercept)){
			dir = Direction::EAST;
		}
	}else if (inPathCornerRange(enemy::corner4X1, enemy::corner4Y1, enemy::corner4X2, enemy::corner4Y2)){
		// checks if the enemy is above the diagonal line
		if (y > ((enemy::corner4Slope *x) + enemy::corner4YIntercept)){
			dir = Direction::SOUTH;
		}
	}else if (inPathCornerRange(enemy::corner5X1, enemy::corner5Y1, enemy::corner5X2, enemy::corner5Y2)){
		// checks if the enemy is above the diagonal line
		if (y > ((enemy::corner5Slope *x) + enemy::corner5YIntercept)){
			dir = Direction::EAST;
		}
	}
}

bool Enemy::inPathCornerRange(int cornerX1, int cornerY1, int cornerX2, int cornerY2){
	// is enemy in between the x and y range of the corner 
	if (((x > std::min(cornerX1, cornerX2)) && (x < std::max(cornerX1, cornerX2))) 
		&&  ((y > std::min(cornerY1, cornerY2)) && (y < std::max(cornerY1, cornerY2)))){
		return true;
	}
	return false;
}