#include <iostream>
#include <stdio.h>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <fstream>

#include "waveManager.h"
#include "weather.h"
#include "enemy.h"
#include "tower.h"
#include "laserTower.h"
#include "barracks.h"
#include "bombTower.h"
#include "constants.h"
#include "initAudio.h"

// Constructor
Logic::Logic() {
    score = 0;
    health = 20;
    paused = false;
    start = true;
    weather = std::make_shared<Weather>();
    wave_manager = std::make_shared<WaveManager>(getWeather());
    moneyManager = std::make_shared<MoneyManager>();
}

bool Logic::updateMoneyTowerAction(const std::string& action, int coinAmount){
    if (action == "Buy" || 
        action == "Upgrade" ||
        action == "Repair" ){
        bool res = moneyManager->spendMoney(coinAmount);
        return res;
    }else if(action == "Sell"){
        moneyManager->gainMoney(coinAmount);
        return true;
    }
    // Should never reach here 
    return false;
}

void Logic::updateTowerWeatherRange(std::shared_ptr<Tower> towerPtr) {
    if (weather->getWeatherName() == "Sandstorm") {
        if (towerPtr->getName() == "LaserTower") {
            towerPtr->setRange(tower::laserRange + weather->getTowerRangeMod());
        } else if (towerPtr->getName() == "BombTower") {
            towerPtr->setRange(tower::bombRange + weather->getTowerRangeMod());
        }
    } else {
        if (towerPtr->getName() == "LaserTower") {
            towerPtr->setRange(tower::laserRange);
        } else if (towerPtr->getName() == "BombTower") {
            towerPtr->setRange(tower::bombRange);
        }
    }
}

void Logic::update(double elapsedTime){
    using namespace window;
    if(isPaused() == false && onStart() == false){
        // Player slowly gains money every update 
        if (moneyManager->isReadyToSlowGain(elapsedTime)){
            moneyManager->slowGain();
        }

        for (int i = 0; i < wave_manager->getActiveEnemies().size(); i ++){
            wave_manager->getActiveEnemies()[i].move();

            if(wave_manager->getActiveEnemies()[i].getX() >= window::screenWidth){
                    takeDamage(wave_manager->getActiveEnemies()[i].getDamage());
                    wave_manager->getActiveEnemies()[i].takeDamage(wave_manager->getActiveEnemies()[i].getHealth());
                    playSound("../resource/Alarm.wav");
            }

            // Check if enemy is dead and remove from list
            if (!wave_manager->getActiveEnemies()[i].isAlive()) {
            	moneyManager->gainMoney(wave_manager->getActiveEnemies()[i].getCoins());
                wave_manager->getActiveEnemies().erase(wave_manager->getActiveEnemies().begin() + i);
                i--;
                continue;
            }
        }
        
        wave_manager->update(elapsedTime);
        weather->updateWeather(elapsedTime);

        score = wave_manager->getCurrWave();

        // Iterate through towers to update their targets and attack
	    for (const TowerLocation& location : TowerLocationManager::getTowerLocations()) {
	        if (location.occupied) {
	            std::shared_ptr<Tower> tower = location.tower;
	            if (tower) {
                    // Update tower degradation
                    if (tower->isReadyToSlowDegrade(elapsedTime)) {
                        tower->slowDegrade(weather->getTowerHpMod());
                    }

                    updateTowerWeatherRange(tower);

                    // Update tower attacking only if tower is not destroyed
                    if (tower->isDestroyed() == false) {
                        if (std::shared_ptr<LaserTower> laserTower = std::dynamic_pointer_cast<LaserTower>(tower)) {
                            if (laserTower->isReadyToAttack(elapsedTime)) {
                                laserTower->updateTarget(wave_manager->getActiveEnemies());
                                laserTower->attack();
                            }
                        } else if (std::shared_ptr<Barracks> barracksTower = std::dynamic_pointer_cast<Barracks>(tower)) {
                            barracksTower->updateTarget(wave_manager->getActiveEnemies());
                            barracksTower->update(elapsedTime);
                        } else if (std::shared_ptr<BombTower> bombTower = std::dynamic_pointer_cast<BombTower>(tower)) {
                            if (bombTower->isReadyToAttack(elapsedTime)) {
                                bombTower->updateTarget(wave_manager->getActiveEnemies());
                            }
                        }
	                } else {
                        if (std::shared_ptr<Barracks> barracksTower = std::dynamic_pointer_cast<Barracks>(tower)) {
                            barracksTower->resumeEnemyMovement();
                        }
                    }
	            }
	        }
        }
    }
}

void Logic::switchStart(bool s){
	start = s;
}

void Logic::reset(){

    //update the score

    //first read the file and add it to a vector
    std::ifstream inputFile("../src/leaderboard.txt");
    if (!inputFile) {
        std::cerr << "Failed to open file." << std::endl;
    }

    std::vector<Score> board;

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string date;
        int score;

        if (!(iss >> date >> score)) {
            std::cerr << "Error reading line from file." << std::endl;
            continue;
        }

    board.emplace_back(date, score);

    }

    //then sort the vector from least to greatest
    std::sort(board.begin(), board.end(), [](const Score& a, const Score& b) {
        return a.getScore() < b.getScore();});
    
    //if the lowest score is less than the current score replace it
    if (board[0].getScore() < score){

        //get the current date
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        // Use stringstream to format the date
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << (now->tm_mon + 1) << '/'
        << std::setfill('0') << std::setw(2) << now->tm_mday << '/'
        << std::setw(2) << (now->tm_year % 100);

        // Convert to string
        std::string dateStr = ss.str();

        board[0] = Score(dateStr, score);
    }

    //write the new scores to the leaderboard file
    
    // Open a file in write mode
    std::ofstream outFile("../src/leaderboard.txt");
    if (!outFile.is_open()) {
        std::cerr << "Unable to open file for writing." << std::endl;
    }

    // Write the scores to the file
    for (const auto& score : board) {
        outFile << score.getDate() << " " << score.getScore() << std::endl;
    }

    // Close the file
    outFile.close();

    //code for resetting the game
	score = 0;
    health = 20;
    paused = false;
    start = true;

    // Resetart everything
    weather = std::make_shared<Weather>();
    wave_manager->reset(getWeather());
    moneyManager->reset();

    // Delete all the towers
    std::vector<TowerLocation>& towerLocations = TowerLocationManager::getTowerLocations();
    for (size_t i = 0; i < towerLocations.size(); ++i) {
		if(towerLocations[i].occupied == true){
	            towerLocations[i].tower = nullptr;
	            towerLocations[i].occupied = false;
		}
    }
}
