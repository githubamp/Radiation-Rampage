#ifndef HUD_H
#define HUD_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class HUD {
private:
    SDL_Renderer* renderer;
    SDL_Rect backgroundRect;
    SDL_Color textColor;
    TTF_Font* font;
    int screenWidth;
    int screenHeight;
    int coinCount;
    int playerHealth;
    int currentWave;

    // Text rendering helper function
    void renderText(const std::string& text, int x, int y);

public:
    HUD(SDL_Renderer* renderer, int screenWidth, int screenHeight);

    // Update HUD with new statistics
    void update(int coins, int health, int wave);

    // Render the HUD on the screen
    void render();
};

#endif