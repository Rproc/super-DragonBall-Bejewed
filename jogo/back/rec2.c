#include "stdio.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "string.h"

int main(int argc, char **argv)
{
	SDL_Surface *screen=NULL;
	SDL_Surface *text=NULL;
	SDL_Surface *fundo=NULL;
	SDL_Rect rect={0,0};
	TTF_Font *font = NULL;
	SDL_Color textColor = { 255, 255, 255 };
	SDL_Event event;

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	
	screen=SDL_SetVideoMode(500,500,16,SDL_SWSURFACE);
  font = TTF_OpenFont( "ARIAL.TTF", 36 );

	SDL_EnableUNICODE( SDL_ENABLE );
	fundo=IMG_Load("fundo2.jpg");

	char temp[20]={' '};
	char a;
	int i=0;
	
	int quit=0;
	SDL_BlitSurface(fundo,NULL,screen,NULL);
	SDL_Flip(screen);
	while(quit==0)
	{
    while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_KEYDOWN )
			{
			    //Keep a copy of the current version of the string
			 		
			    //If the string less than maximum size
			    if( strlen(temp) <= 19 )
			    {
			        //If the key is a space
			        if( event.key.keysym.unicode == (Uint16)' ' )
			        {
			            //Append the character
			            temp[i] = (char)event.key.keysym.unicode;
									i++;
			        }
			        //If the key is a number
			        else if( ( event.key.keysym.unicode >= (Uint16)'0' ) && ( event.key.keysym.unicode <= (Uint16)'9' ) )
			        {
			            //Append the character
			            temp[i] = (char)event.key.keysym.unicode;
									i++;
			        }
			        //If the key is a uppercase letter
			        else if( ( event.key.keysym.unicode >= (Uint16)'A' ) && ( event.key.keysym.unicode <= (Uint16)'Z' ) )
			        {
			            //Append the character
			            temp[i] = (char)event.key.keysym.unicode;
									i++;
			        }
			        //If the key is a lowercase letter
			        else if( ( event.key.keysym.unicode >= (Uint16)'a' ) && ( event.key.keysym.unicode <= (Uint16)'z' ) )
			        {
			            //Append the character
			            temp[i] = (char)event.key.keysym.unicode;
									i++;
			        }
			    }

			    //If backspace was pressed and the string isn't blank
			    if( ( event.key.keysym.sym == SDLK_BACKSPACE ) && ( i != 0 ) )
			    {
			        //Remove a character from the end
			        i--;
							temp[i]=' ';
			    }

			        //Render a new text surface
			        text = TTF_RenderText_Solid( font, temp, textColor );
			    }
	          if( event.type == SDL_QUIT )
			  {
                //Quit the program
                quit = 1;
           	   }
				}
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_BlitSurface(text,NULL,screen,&rect);
		SDL_Delay(1000/60);
		SDL_Flip(screen);
	}
	
	
	return 0;
}
