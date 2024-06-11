#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <SDL.h>
#include <random>
#include <memory>

#include "enemy.h"
#include "waveManager.h"

//constructor
WaveManager::WaveManager(std::shared_ptr<Weather> weatherpntr) {
    //setting timing
    time_til_next_wave = 1;

    weather = weatherpntr;

    currWave = 0;

    //create list of enemies
    enemies = createEnemies();

    SDL_Init( SDL_INIT_TIMER );

    setEnenmyWaves();
}

std::vector<Enemy>& WaveManager::getActiveEnemies() {
    return active_enemies;
}

Direction WaveManager::stringToDirection(const std::string& str) {
    if (str == "NORTH") return Direction::NORTH;
    if (str == "SOUTH") return Direction::SOUTH;
    if (str == "EAST") return Direction::EAST;
    if (str == "WEST") return Direction::WEST;

    return Direction::SOUTH; // Default value
}

std::vector<Enemy> WaveManager::createEnemies(){
    std::ifstream inputFile("../src/stats.txt");
    if (!inputFile) {
        std::cerr << "Failed to open file." << std::endl;
    }

    std::vector<Enemy> enemies;

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string name;
        int health, speed, x, y, damage, coins;
        std::string direct_str, flying_str; // Read as strings

        if (!(iss >> name >> health >> speed >> x >> y >> direct_str >> damage >> flying_str >> coins)) {
            std::cerr << "Error reading line from file." << std::endl;
            continue;
        }

        // Convert strings to enums
        Direction direct = stringToDirection(direct_str);

        // Convert string to boolean
        bool flying = (flying_str == "TRUE");

        enemies.emplace_back(name, health, speed, x, y, direct, damage, flying, coins, weather);
    }
    
    return enemies;
}

// Callback function to be called by the timer
Uint32 WaveManager::TimerCallback(Uint32 interval, void* wave) {
    //case void* back to enemy
    WaveManager* manager = static_cast<WaveManager*>(wave);
    manager->active_enemies.push_back(manager->enemies_to_add.back());
    manager->enemies_to_add.pop_back();
    
    // Return 0 to stop the timer from repeating
    return 0;
}

void WaveManager::update(double elapsedTime) {
    //subtract the elapsed time (in seconds)
    time_til_next_wave -= elapsedTime;
    if (time_til_next_wave < 1) {
        // Pop the last element from enemy waves
        if (!enemy_waves.empty()) {
	        enemies_to_add = enemy_waves.back();
	        enemy_waves.pop_back();
        }
        else{
            enemies_to_add = waveAlgorithm();
        }
        // Update wave counter
        currWave += 1;

        //add the popped waves to active enemies
        // Iterate through wave_to_add and add elements to active_enemies with some delay
        Uint32 delay = 1000;
        for (Enemy& elem : enemies_to_add) {
            // Set the timer to trigger after 1000 milliseconds (1 seconds) per enemeny
            SDL_AddTimer(delay, TimerCallback, this);
            delay += 1000;
        }
        //reset clock
        time_til_next_wave = time_between_waves;
    }

}

std::vector<Enemy> WaveManager::waveAlgorithm(){
    //add a number of enemies equal to what wave it is
    //which enemies are created is randomised
    std::vector<Enemy> ranWave;

    // Create a random number generator engine
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution<int> distrib(0, 7); //add a number between 1 and 8
    
    int i = 0;
    while (i < currWave){
        int randomNumber = distrib(gen); // generate the number
        ranWave.push_back(enemies[randomNumber]);
        i++;
    }

    return ranWave;
}

void WaveManager::reset(std::shared_ptr<Weather> w){
    //pass weather
    weather = w;

	enemy_waves.clear();
	active_enemies.clear();

	time_til_next_wave = 1;

    currWave = 0;

    setEnenmyWaves();
}

void WaveManager::setEnenmyWaves() {
    //create first 10 waves of enemies
    std::vector<Enemy> wave1;
        wave1.push_back(enemies[0]);
        wave1.push_back(enemies[0]);
        wave1.push_back(enemies[0]);
        wave1.push_back(enemies[0]);
        wave1.push_back(enemies[0]);
    std::vector<Enemy> wave2;
        wave2.push_back(enemies[0]);
        wave2.push_back(enemies[0]);
        wave2.push_back(enemies[1]);
        wave2.push_back(enemies[0]);
        wave2.push_back(enemies[0]);
        wave2.push_back(enemies[0]);
        wave2.push_back(enemies[1]);
    std::vector<Enemy> wave3;
        wave3.push_back(enemies[2]);
        wave3.push_back(enemies[2]);
        wave3.push_back(enemies[1]);
        wave3.push_back(enemies[2]);
        wave3.push_back(enemies[2]);
        wave3.push_back(enemies[2]);
        wave3.push_back(enemies[1]);
    std::vector<Enemy> wave4;
        wave4.push_back(enemies[4]);
        wave4.push_back(enemies[4]);
        wave4.push_back(enemies[1]);
        wave4.push_back(enemies[0]);
        wave4.push_back(enemies[0]);
        wave4.push_back(enemies[4]);
        wave4.push_back(enemies[5]);
    std::vector<Enemy> wave5;
        wave5.push_back(enemies[6]);
        wave5.push_back(enemies[6]);
        wave5.push_back(enemies[1]);
        wave5.push_back(enemies[6]);
        wave5.push_back(enemies[6]);
        wave5.push_back(enemies[6]);
        wave5.push_back(enemies[1]);
    std::vector<Enemy> wave6;
        wave6.push_back(enemies[3]);
        wave6.push_back(enemies[3]);
        wave6.push_back(enemies[3]);
        wave6.push_back(enemies[3]);
        wave6.push_back(enemies[3]);
        wave6.push_back(enemies[0]);
        wave6.push_back(enemies[0]);
        wave6.push_back(enemies[0]);
    std::vector<Enemy> wave7;
        wave7.push_back(enemies[1]);
        wave7.push_back(enemies[0]);
        wave7.push_back(enemies[0]);
        wave7.push_back(enemies[0]);
        wave7.push_back(enemies[0]);
        wave7.push_back(enemies[0]);
        wave7.push_back(enemies[0]);
        wave7.push_back(enemies[0]);
        wave7.push_back(enemies[0]);
        wave7.push_back(enemies[0]);
        wave7.push_back(enemies[0]);
    std::vector<Enemy> wave8;
        wave8.push_back(enemies[3]);
        wave8.push_back(enemies[3]);
        wave8.push_back(enemies[3]);
        wave8.push_back(enemies[3]);
        wave8.push_back(enemies[3]);
        wave8.push_back(enemies[7]);
        wave8.push_back(enemies[7]);
        wave8.push_back(enemies[7]);
        wave8.push_back(enemies[7]);
        wave8.push_back(enemies[7]);
    std::vector<Enemy> wave9;
        wave9.push_back(enemies[7]);
        wave9.push_back(enemies[1]);
        wave9.push_back(enemies[7]);
        wave9.push_back(enemies[7]);
        wave9.push_back(enemies[7]);
        wave9.push_back(enemies[1]);
        wave9.push_back(enemies[1]);
        wave9.push_back(enemies[7]);
        wave9.push_back(enemies[1]);
    std::vector<Enemy> wave10;
        wave10.push_back(enemies[5]);
        wave10.push_back(enemies[5]);
        wave10.push_back(enemies[5]);
        wave10.push_back(enemies[5]);
        wave10.push_back(enemies[5]);
        wave10.push_back(enemies[5]);
        wave10.push_back(enemies[5]);
        wave10.push_back(enemies[5]);
        wave10.push_back(enemies[5]);

    //push back waves in reverse order so they can easily be poped off
    enemy_waves.push_back(wave10);
    enemy_waves.push_back(wave9);
    enemy_waves.push_back(wave8);
    enemy_waves.push_back(wave7);
    enemy_waves.push_back(wave6);
    enemy_waves.push_back(wave5);
    enemy_waves.push_back(wave4);
    enemy_waves.push_back(wave3);
    enemy_waves.push_back(wave2);
    enemy_waves.push_back(wave1);
}
