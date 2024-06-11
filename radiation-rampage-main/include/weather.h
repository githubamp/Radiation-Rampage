#ifndef WEATHER_H
#define WEATHER_H

#include <iostream>
#include <stdio.h>

class Weather {
    private:
        std::string name;
        int towerHpMod;//number to multiply to tower degrade
        int towerRangeMod; // number to add tower range to
        int enemyHpMod; //number to divide damadge by
        int enemySpeedMod; //number to add to speed

        //how many seconds between weather
        const double time_between_weather = 30;
        double time_til_next_weather;

        //set the weather to diffrent types
        void weatherSetNone();
        void weatherSetAcidRain();
        void weatherSetSandstorm();
        void weatherSetRadiation();
        void weatherSetEarthquake();
        void weatherSetWindEast();
        void weatherSetWindWest();

    public:
        //constructor
        Weather();

        //randomly sets a weather
        void updateWeather(double elapsedTime);

        //getters for the diffrent modifiers
        float  getTowerHpMod() const { return towerHpMod; }
        float getTowerRangeMod() const { return towerRangeMod; };
        float getenemyHpMod() const { return enemyHpMod; };
        float getenemySpeedMod() const { return enemySpeedMod; };
        std::string getWeatherName() const { return name; };
        
        // Reset method
		void reset();
};

#endif