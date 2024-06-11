#include "logic.h"
#include "view.h"

#include "initAudio.h"

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <stdio.h>

int main(int argc, char** argv)
{
  /*** Initialization ***/

  SDL_Init( SDL_INIT_VIDEO );
  initAudio();
  
  Logic logic = Logic();
  View view = View();

  /*** Main Loop ***/
  bool running = true;

  unsigned int prevTicks = 0;
  unsigned int ticks = 0;
  double time;


  // While application is running
  while( running )
  {
    // Get ticks since last loop
    ticks = SDL_GetTicks() -prevTicks;
    prevTicks = SDL_GetTicks();
    // Turn ticks into seconds
    time = ticks/1000.0;

    // Run updates
    logic.update(time);
    running = view.update(logic);

    // Delay so it isnt too fast
    SDL_Delay(10);
  }

  // Quit SDL subsystems
  SDL_Quit();
  
  return 0;
}