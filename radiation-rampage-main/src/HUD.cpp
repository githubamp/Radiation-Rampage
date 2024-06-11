#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "HUD.h"

HUD::HUD(SDL_Renderer* renderer, int screenWidth, int screenHeight) 
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight),
      coinCount(0), playerHealth(100), currentWave(1) {

    // Initialize background rectangle
    backgroundRect.w = 400; // Width of the rectangle
    backgroundRect.h = 50;  // Height of the rectangle
    backgroundRect.x = (screenWidth - backgroundRect.w) / 2; // Center horizontally
    backgroundRect.y = screenHeight - 50; // 100 pixels from the bottom

    // Set text color
    textColor = { 255, 255, 255, 255 }; // White color

    // Load font
    if (TTF_Init() == -1) {
        std::cerr << "Error initializing SDL_ttf: " << TTF_GetError() << std::endl;
    }
    font = TTF_OpenFont("../resource/arial.ttf", 24); // Replace "path/to/your/font.ttf" with the actual path to your font file
    if (font == NULL) {
        std::cerr << "Error loading font: " << TTF_GetError() << std::endl;
    }
}

void HUD::update(int coins, int health, int wave) {
    coinCount = coins;
    playerHealth = health;
    currentWave = wave;
}

void HUD::render() {
    // Render background rectangle
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color
    SDL_RenderFillRect(renderer, &backgroundRect);

    // Load images for coins and health directly into textures
    SDL_Texture* coinTexture = IMG_LoadTexture(renderer, "../resource/Coin.png");
    SDL_Texture* healthTexture = IMG_LoadTexture(renderer, "../resource/Heart.png");

    if (coinTexture == NULL || healthTexture == NULL) {
        std::cerr << "Error loading images: " << SDL_GetError() << std::endl;
        return;
    }

    // Render coin image
    SDL_Rect coinRect = { backgroundRect.x + 10, backgroundRect.y + 5, 40, 40 };
    SDL_RenderCopy(renderer, coinTexture, NULL, &coinRect);

    // Render health image
    SDL_Rect healthRect = { backgroundRect.x + 150, backgroundRect.y, 50, 50 };
    SDL_RenderCopy(renderer, healthTexture, NULL, &healthRect);
    
    // Render text for coin count
    std::string coinText = ": " + std::to_string(coinCount);
    renderText(coinText, backgroundRect.x + 55, backgroundRect.y + 10);

    // Render text for health
        std::string healthText = "";
    if(playerHealth < 0){
	healthText = ": 0";
    }else{
    	healthText = ": " + std::to_string(playerHealth);
    }
    renderText(healthText, backgroundRect.x + 200, backgroundRect.y + 10);

    // Render wave text
    std::string waveText = "Wave: " + std::to_string(currentWave);
    renderText(waveText, backgroundRect.x + 270, backgroundRect.y + 10);

    // Clean up
    SDL_DestroyTexture(coinTexture);
    SDL_DestroyTexture(healthTexture);
}

void HUD::renderText(const std::string& text, int x, int y) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), textColor);
    if (surface == NULL) {
        std::cerr << "Error rendering text: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        std::cerr << "Error creating texture from surface: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect textRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
