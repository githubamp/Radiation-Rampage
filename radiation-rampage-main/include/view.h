#ifndef VIEW_H
#define VIEW_H

#include <SDL.h>
#include <stdio.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>
#include <memory>

#include "logic.h"
#include "HUD.h"
#include "startScreen.h"
#include "weather.h"
#include "damage_type.h"

class TOWERGUI;
struct TowerLocation;
struct FailedTransMessage;

struct AttackAnimation {
    bool active;
    int startX;
    int startY;
    int endX;
    int endY;
    Uint32 startTime;
};

struct FailedTransMessage{
    // Message box displayed when a money transaction fails
    bool active = false;
    Uint32 startTime;
};

class View {
private:
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;

    SDL_Window* window;
    SDL_Renderer* renderer;

    std::shared_ptr<Logic> logic;
    std::shared_ptr<TOWERGUI> tower_gui;
    std::shared_ptr<TOWERGUI> update_tower_gui;
    std::shared_ptr<HUD> hud;
    std::shared_ptr<startScreen> start;

    AttackAnimation laserAttackAnimation;
    AttackAnimation bombAttackAnimation;
    FailedTransMessage failedTransMessage;

    SDL_Texture* barracksTexture;
    SDL_Texture* bombTexture;
    SDL_Texture* laserTexture;
    SDL_Texture* barracksSoldierTexture;
    SDL_Texture* barracksUpgradeTexture;
    SDL_Texture* bombUpgradeTexture;
    SDL_Texture* laserUpgradeTexture;
    SDL_Texture* bombEffectTexture;

    SDL_Texture* humanRaiderTexture;
    SDL_Texture* hawkNorthTexture;
    SDL_Texture* mutantHawkNorthTexture;
    SDL_Texture* hawkSouthTexture;
    SDL_Texture* mutantHawkSouthTexture;
    SDL_Texture* hawkEastTexture;
    SDL_Texture* mutantHawkEastTexture;
    SDL_Texture* dogTexture;
    SDL_Texture* mutantDogTexture;
    SDL_Texture* humanRaiderLeaderTexture;
    SDL_Texture* mutantRaiderTexture;
    SDL_Texture* mutantLeaderTexture;

    void loadTowerTextures();
    void loadEnemyTextures();
    void renderFailedTransMessage();
    
public:
    View();
    ~View();
    bool update(Logic& logic);
    
    void renderEnemies(const std::vector<Enemy>& enemies);
    void renderGUI();
    void renderTowerLocations();
    void renderHUD(const Logic& logic);
    void renderTowerRadius(const TowerLocation& location);
    void renderSoldiers();
    void renderLost(Logic& logic);
    void renderPause();
    void handleStartScreen(const SDL_Event& event);
    void renderWaveTime(const WaveManager& manager);
    void handleTowerClick(const SDL_Event& event);
    void handleTowerTypeSelection(const SDL_Event& event, Logic& logic);
    void triggerAttackAnimation(int startX, int startY, int endX, int endY, DamageType attackType);
    void renderAttackAnimation(DamageType attackType);
    void renderWeatherName(const Weather& weather);
    void renderWeatherTint(const Weather& weather);
};
#endif
