#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>

int main() {
  SDL_Surface* image = NULL;
  SDL_Surface* screen = NULL;
  SDL_Event e;
  bool quit = false;

  SDL_Init(SDL_INIT_EVERYTHING);

  screen = SDL_SetVideoMode(540,480,32, SDL_SWSURFACE);

  image = IMG_Load("cichon.jpg");
  SDL_BlitSurface(image, NULL, screen, NULL);
  
  SDL_Flip(screen);
  while(!quit) {
    while(SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }
  }

  SDL_FreeSurface(image);

  SDL_Quit();

  return 0;
}
