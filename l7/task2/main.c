/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define WIDTH  800
#define HEIGHT 600

void Slock();
void Sulock();

void Slock(SDL_Surface* screen) {
  if (SDL_MUSTLOCK(screen))
    if(SDL_LockSurface(screen)  < 0)
      return;
}

void Sulock(SDL_Surface* screen) {
  if (SDL_MUSTLOCK(screen))
    SDL_UnlockSurface(screen);
}

void fade(SDL_Surface* img1, SDL_Surface* img2, Uint8 alpha, SDL_Surface* output_img) {
  int pixels = WIDTH * HEIGHT;

  Uint32* A = (Uint32*) img1->pixels;
  Uint32* B = (Uint32*) img2->pixels;
  Uint32* out = (Uint32*) output_img->pixels;
  Uint32* end = out + pixels;

  int alpha1 = 0;
  int alpha2 = 0;

  for (int i=0; i<4; i++) {
    alpha1 <<= 8;
    alpha1 += alpha;
    alpha2 <<= 8;
    alpha2 += 128-alpha;
  }

  for(; out != end; out++, A++, B++) {
    __asm__("movd %[alpha1], %%mm3\n\t"
            "movd %[alpha2], %%mm4\n\t"
            "movd %[A], %%mm1\n\t"
            "movd %[B], %%mm2\n\t"
            "pxor %%mm0, %%mm0\n\t"
            "punpcklbw %%mm0, %%mm1\n\t"
            "punpcklbw %%mm0, %%mm2\n\t"
            "punpcklbw %%mm0, %%mm3\n\t"
            "punpcklbw %%mm0, %%mm4\n\t"
            "pmullw %%mm3, %%mm1\n\t"
            "pmullw %%mm4, %%mm2\n\t"
            "paddw %%mm2, %%mm1\n\t"
            "psrlw $7, %%mm1\n\t"
            "packuswb %%mm0, %%mm1\n\t"
            "movd %%mm1, %[out]\n\t"
            : [out] "=m" (*out)
            : [alpha1]"m" (alpha1), [alpha2] "m" (alpha2), [A] "m"(*A), [B] "m" (*B)
            : "%mm0", "%mm1", "%mm2", "%mm3", "%mm4");
  }
  __asm__("emms" : : );
}



int main( int argc, char* args[] ) {
  char* file1 = "image1.jpg";
  char* file2 = "image2.jpg";
   
  if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ){
      printf("Unable to init SDL: %s\n", SDL_GetError());
      exit(1);
  }
  atexit(SDL_Quit);
   
  SDL_Surface *screen;
  screen = SDL_SetVideoMode( WIDTH, HEIGHT,32, SDL_HWSURFACE | SDL_DOUBLEBUF);
   
  if ( screen == NULL ){
      printf("Unable to set %dx%d video: %s\n",800, 600, SDL_GetError());
      exit(1);
  }
 
  SDL_Surface *image1,*image2,*output;
  SDL_Surface *temp;
     
  temp = IMG_Load(file1);
  if (temp == NULL)
  {
      printf("Unable to load bitmap: %s\n", SDL_GetError());
      return 1;
  }
  image1 = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);

  temp = IMG_Load(file2);
  if (temp == NULL)
  {
      printf("Unable to load bitmap: %s\n", SDL_GetError());
      return 1;
  }
  image2 = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);

  temp = SDL_CreateRGBSurface(
        SDL_SWSURFACE, WIDTH, HEIGHT,
        image1->format->BitsPerPixel,
        image1->format->Rmask, 
        image1->format->Gmask,
        image1->format->Bmask, 
        image1->format->Amask
    );
   
  output = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);

  bool quit;
  SDL_Event e;
  int f;


  SDL_Rect src, dest;
    
  src.x = 0;
  src.y = 0;
  src.w = output->w;
  src.h = output->h;
   
  dest.x = 0;
  dest.y = 0;
  dest.w = output->w;
  dest.h = output->h;
  
  int maxFPS = 0;
  int FPS = 0;
  int lastTimeFPS = 0;
  
  int done = false;
  int mode = 1, lastMode = 0;
  char buffer[32];

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    int alpha = f<0x80 ? (f&0x7f): 0x80-(f&0x7f);
    f++;
    fade(image1, image2, alpha, output);

    int currentTime = SDL_GetTicks();

    Slock(screen);
    SDL_BlitSurface(output, NULL, screen, NULL);
    Sulock(screen);

    SDL_Flip(screen);


    FPS++;
    
    if ( currentTime - lastTimeFPS >= 1000 )
    {
        if (FPS > maxFPS)
            maxFPS = FPS;
        
        if (lastMode != mode)
            maxFPS = 0;
        
        snprintf( buffer, sizeof(buffer), "%d FPS (max:%d) [%d]", FPS, maxFPS, mode );
        SDL_WM_SetCaption( buffer,0 );
        FPS = 0;
        lastTimeFPS = currentTime;
        lastMode = mode;
    } 
  }

  SDL_Quit();

	return 0;
}
