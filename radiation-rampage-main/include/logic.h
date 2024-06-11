#ifndef LOGIC_H
#define LOGIC_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <memory>

#include "waveManager.h"
#include "enemy.h"
#include "moneyManager.h"
#include "weather.h"

class Tower;

class Logic {
private:
    int score;
    int health;
    double time;
    bool game_over; // Tells view if the game is over
    bool paused;	// Tells other views if the game is paused
    bool start;		// On start screen

    std::shared_ptr<WaveManager> wave_manager;
    std::shared_ptr<MoneyManager> moneyManager;
    std::shared_ptr<Weather> weather;

public:
    // Constructor, create the wave manager and set game_over and paused to false
    Logic();
    
    // Update money manager based on tower action. Returns true if transaction went through, false if it didn't
    bool updateMoneyTowerAction(const std::string& action, int coinAmount);

    // Update tower range based on weather
    void updateTowerWeatherRange(std::shared_ptr<Tower> towerPtr);

    // Update method
    void update(double inTime);

    // Reset the game for new session
    void reset();
    
    // Change the game between start and play screen
    void switchStart(bool s);

    // Getters
    int getScore() const { return score; }

    int getHealth() const { return health; }

    int getMoney() const { return moneyManager->getMoney(); }

    bool isPaused() const { return paused; }
    
    bool onStart() const { return start; }

    std::shared_ptr<WaveManager> getManager() const { return wave_manager; }

    std::shared_ptr<Weather> getWeather() const { return weather; }

    std::vector<Enemy> getEnemiesOnField() const { return wave_manager->getActiveEnemies(); }

    // Setters
    void setPaused() { paused = true; }

    void setUnpaused() { paused = false; }

    void takeDamage(int d){ health -= d; }
};

#endif