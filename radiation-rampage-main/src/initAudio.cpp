#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include "initAudio.h"

void initAudio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        exit(1);
    }
}

void playSound(const char* soundFile) {
    Mix_Chunk* soundEffect = Mix_LoadWAV(soundFile);
    if (soundEffect == NULL) {
        std::cerr << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        exit(1);
    }

    Mix_PlayChannel(-1, soundEffect, 0);
}