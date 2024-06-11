#ifndef WAVEMANAGER_H
#define WAVEMANAGER_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <SDL.h>
#include <memory>

#include "enemy.h"

class WaveManager {
private:
    //list of enemies actively on the battlefield
    std::vector<Enemy> active_enemies;
    
    //list of list of enemies in each wave
    std::vector<std::vector<Enemy>> enemy_waves;

    //list of every enemy type
    std::vector<Enemy> enemies;

    //this is need to be stored in wave manager because the timer callback needs to be static
    std::vector<Enemy> enemies_to_add;

    //how many seconds between waves
    const int time_between_waves = 25;
    double time_til_next_wave;
    
    // Current wave
    int currWave;

    // Assist in parsing text to enums
    Direction stringToDirection(const std::string& str);
    
    //weather for mods to enemys
    std::shared_ptr<Weather> weather;

    void setEnenmyWaves();

public:
    //constructor, create the first 10 waves and add them to the enemy waves, and set time bwtween waves. also sets the weather pointer to the one saved in logic
    WaveManager(std::shared_ptr<Weather> weatherpntr);

    //gives list of enemies on the field
    std::vector<Enemy>& getActiveEnemies();

    static Uint32 TimerCallback(Uint32 interval, void* wave);

    //update called in logic
    //moves time until next wave down and sends enemy out if it hits 0
    void update(double elapsedTime);

    //algorithm for generating waves past wave 10
    std::vector<Enemy> waveAlgorithm();

    // Create vector of all enemy types
    std::vector<Enemy> createEnemies();
    
    // Reset method
    void reset(std::shared_ptr<Weather> w);

    // Getter
    double getWaveTime() const { return time_til_next_wave; }
    int getCurrWave() const { return currWave; }
};


#endif