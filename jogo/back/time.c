#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

SDL_Surface *startStop = NULL;
SDL_Surface *seconds = NULL;
SDL_Surface *screen = NULL;


//The event structure
SDL_Event event;

//The font
TTF_Font *font = NULL;

void itoa(int n, char s[])
{
   	int i, sign;

   if ((sign = n) < 0) 
       n = -n;
   i = 0;
   do {       /* generate digits in reverse order */
       s[i++] = n % 10 + '0';   /* get next digit */
   } while ((n /= 10) > 0);     /* delete it */
   if (sign < 0)
       s[i++] = '-';
   s[i] = '\0';
   reverse(s);
}

void reverse(char s[])
{
   int i, j;
   char c;

   for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
       c = s[i];
       s[i] = s[j];
       s[j] = c;
   }
}

void clean_up()
{
    //Free the surfaces
    SDL_FreeSurface( startStop );

    //Close the font
    TTF_CloseFont( font );

    //Quit SDL_ttf
    TTF_Quit();

    //Quit SDL
    SDL_Quit();
}

int main(int argc,char **argv)
{
	int running = 1;
	int quit = 0;

	char buf[10];
	SDL_Init(SDL_INIT_EVERYTHING);
    screen = SDL_SetVideoMode( 800, 640, 16, SDL_SWSURFACE );

	TTF_Init();
	font = TTF_OpenFont ("ARIAL.TTF", 16);

    Uint32 start = 0;
	SDL_Color textColor = { 255, 255, 255 };

    startStop = TTF_RenderText_Solid( font, "Press S to start or stop the timer", textColor );

    start = SDL_GetTicks();

    while( quit == 0 )
    {
        //While there's an event to handle
        while( SDL_PollEvent( &event ) )
        {
            //If a key was pressed
            if( event.type == SDL_KEYDOWN )
            {
                //If s was pressed
                if( event.key.keysym.sym == SDLK_s )
                {
                    //If the timer is running
                    if( running == 1 )
                    {
                        //Stop the timer
                        running = 0;
                        start = 0;
                    }
                    else
                    {
                        //Start the timer
                        running = 1;
                        start = SDL_GetTicks();
                    }
                }
            }

            //If the user has Xed out the window
            else if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = 1;
            }
        }

	 	SDL_BlitSurface( ( 640 - startStop->w ) / 2, 200, startStop, screen );

        if( running == 1 )
		{
			itoa(SDL_GetTicks, buf);

			seconds = TTF_RenderText_Solid( font, buf, textColor);

	 		SDL_BlitSurface( ( 640 - seconds->w ) / 2, 50, seconds, screen );
		    SDL_FreeSurface( seconds );
		}

		SDL_Flip( screen );

   		clean_up();
	return 0;
	}

}


