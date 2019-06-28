#include <SDL/SDL.h>
#include "time.h"
 
int main(int argc, char* argv[])
{
    
 
    /* A pointer to the video surface */
    SDL_Surface* screen = NULL;

    SDL_Event event;

    /* Create a double buffered window 640 pixels wide and 480 pixels long */
    screen = SDL_SetVideoMode(800, 640, 16, SDL_HWSURFACE|SDL_DOUBLEBUF);
 
	SDL_Init(SDL_INIT_EVERYTHING);

 
    /* Create a 2D 3 X 3 array of surfaces */
    SDL_Surface* boxes[3][3];
	int running = 1;
 
    register int x;
    register int y;
 
    for (x = 0; x < 3; x++)
    {
        for (y = 0; y < 3; y++)
        {
            /* Create each surface */
            SDL_PixelFormat* format = screen->format;
            boxes[x][y] = SDL_CreateRGBSurface(SDL_HWSURFACE, 80, 80,format->BitsPerPixel,format->Rmask, format->Gmask, format->Bmask, format->Amask);
 
            /* Fill each surface with a random color */
            Uint32 color = SDL_MapRGB(screen->format, 255, 255, 255);
 
            SDL_FillRect(boxes[x][y], NULL, color);
 
            /* Blit each surface on the screen */
            SDL_Rect pos = { x * 80, y * 80 };
            SDL_BlitSurface(boxes[x][y], NULL, screen, &pos);
        }
    }
 
    /* Flip the screen */
    SDL_Flip(screen);
 
	SDL_Delay (10000);
     
    /* Destroy all surfaces in the array */
    for (x = 0; x < 3; x++)
        for (y = 0; y < 3; y++)
            SDL_FreeSurface(boxes[x][y]);
 
    /* Destroy the video surface */
    SDL_FreeSurface(screen);
 
    return 0;
}

