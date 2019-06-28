#include"SDL/SDL.h"
#include"SDL/SDL_image.h"
#include"stdio.h"
#include "time.h"

#define true 1
#define false 0

int npecas =7;

typedef struct
{
	SDL_Surface *image;
	SDL_Rect rect;
	int tipo;
}PECA;

void PreencheMatriz(int matriz[8][8]);
void blitar(PECA peca,SDL_Surface **screen,int matriz[8][8]);

int main(int argc,char **argv)
{
	SDL_Surface *screen;
	SDL_Surface *background;
	PECA peca;
	int	principal[8][8];
	SDL_Event event;
	int running=true;

	background=IMG_Load("Namek2.png");

	PreencheMatriz(principal);

	SDL_Init(SDL_INIT_EVERYTHING);
	screen=SDL_SetVideoMode(800,640,16,SDL_SWSURFACE);

	while(running)
	{
		while(SDL_PollEvent(&event))
		{
			if( event.type==SDL_QUIT)
				running=false;

			if( event.type == SDL_MOUSEBUTTONDOWN )
		    {
        		//If the left mouse button was pressed
    	    	if( event.button.button == SDL_BUTTON_LEFT )
    	   		{
					printf("click! %d  %d\n",event.button.x,event.button.y);
				}
			}
		}
		SDL_BlitSurface(background,NULL,screen,NULL);
		blitar(peca,&screen,principal);
		SDL_Flip(screen);
	}

	return 0;
}

void blitar(PECA peca,SDL_Surface **screen,int matriz[8][8])
{
int i,j;
SDL_Rect src;
	src.x=0;
	src.y=0;

	//peca.image=IMG_Load("joia1.tga");
	peca.rect.x=280;
	peca.rect.y=200;
	peca.tipo=0;

	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			if(matriz[i][j]==0)
				peca.image=IMG_Load("joia1.tga");
			else if(matriz[i][j]==1)
				peca.image=IMG_Load("joia2.tga");
			else if(matriz[i][j]==2)
				peca.image=IMG_Load("joia3.tga");
			else if(matriz[i][j]==3)
				peca.image=IMG_Load("joia4.tga");
			else if(matriz[i][j]==4)
				peca.image=IMG_Load("joia5.tga");
			else if(matriz[i][j]==5)
				peca.image=IMG_Load("joia6.tga");
			else if(matriz[i][j]==6)
				peca.image=IMG_Load("joia7.tga");
		
			SDL_SetAlpha(*screen, SDL_SRCALPHA, 30);
			SDL_BlitSurface(peca.image,&src,*screen,&peca.rect);
			peca.rect.x+=peca.image->w+5;

		}

		peca.rect.y+=peca.image->h+5;
		peca.rect.x=280;

	}
}


void PreencheMatriz(int matriz[8][8])
{
	int i,j;

	srand(time(NULL));
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			matriz[i][j]=rand() %npecas;
		
			if((i==0 || i==1) && (j>=2 && j<8))//verificar primeira e segunda linhas
			{
				while((matriz[i][j]==matriz[i][j-1]) && (matriz[i][j]==matriz[i][j-2]))
				{
					matriz[i][j]=rand() %npecas;
				}
			}
			else// if(i>1 && j>1 && j<8)//verificar outras
			{			
					while( ( (matriz[i][j]==matriz[i][j-1]) && (matriz[i][j]==matriz[i][j-2]) ) || ( (matriz[i][j]==matriz[i-1][j]) && (matriz[i][j]==matriz[i-2][j]) ) )
					{
						matriz[i][j]=rand() %npecas;
					}
			}
		}
	}

}
