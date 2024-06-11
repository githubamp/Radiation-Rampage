#ifndef AUDIOINIT_H
#define AUDIOINIT_H

// Function to initialize SDL_mixer
//made here because it needs to be done across multiple files
void initAudio();

// Function to play a sound effect
void playSound(const char* soundFile);

#endif // AUDIOINIT_H