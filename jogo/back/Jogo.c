#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"

#include "stdio.h"
#include "time.h"
#include "string.h"
#include "stdlib.h"

#define MAX_ARRAY 10
#define MAX_NOME 50
#define true 1
#define false 0

typedef struct
{
	int pontuacao;
	char nome[100];
}RECORDES;

typedef struct{
	RECORDES vetor[10];
	int n;
}R_ARRAY;


typedef struct
{
	SDL_Rect rect;
	int tipo;
}PECA;

typedef struct
{
	PECA peca1;
	PECA peca2;
	int x,y;
	int i1,j1; //posicoes relativas na matriz
	int i2,j2;
}MOUSE;

int EXIT = false;
int npecas =7;
int movimentos=15;
int pontuacao=0;
int comb=0;

SDL_Surface *screen;
SDL_Surface *joia;
SDL_Surface *fundo=NULL;
SDL_Surface *radar=NULL;
SDL_Surface *img=NULL;
Mix_Chunk *Som_de_Troca=NULL;
Mix_Chunk *Som_de_quebra=NULL;
Mix_Music *music=NULL;
RECORDES elemento(R_ARRAY *, int);

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination);
void recorde(R_ARRAY *recordes,int valor,char *nome);
int tamanho(R_ARRAY *);
void insere(R_ARRAY *, RECORDES);
void remove_posicao(R_ARRAY *, int);
void imprime(R_ARRAY *);
void sort(R_ARRAY *);
void bubble_sort(RECORDES *, int);
void Menu();
void PreencheMatriz(PECA peca[8][8]);
void blitar(PECA peca[8][8]);
int ColideMouse(SDL_Rect r1, MOUSE mouse);
void VerificaTroca(PECA peca[8][8],MOUSE *mouse);
void Troca(MOUSE *mouse,PECA peca[8][8]);
int Combina(PECA peca[8][8]);
void quebra(PECA peca[8][8]);
void sobe(PECA peca[8][8]);
void VerificaMatriz(PECA peca[8][8]);
void Score(PECA peca[8][8]);
void dica(PECA matriz[8][8]);
void CarregaImagem (PECA matriz[8][8],int i, int j);
void itoa(int n, char s[]);
void reverse(char s[]);
void BlitaPontos ();
void BlitaTempo ();

void Efeito_Troca(PECA peca[8][8],MOUSE *mouse);
void Troca_Vertical(PECA peca[8][8],MOUSE *mouse);
void Troca_Horizontal(PECA peca[8][8],MOUSE *mouse);
void Explode(PECA peca[8][8]);
void EfeitoCai(PECA peca[8][8]);
void Desce(PECA matriz[8][8], int i, int j);
void Efeito_Recorde(R_ARRAY *recordes);
void ApresentaRecordes();

int main(int argc,char **argv)
{
	PECA peca[8][8];
	
	SDL_Event event;
	MOUSE mouse;
	R_ARRAY recordes;

	mouse.peca1.tipo=-2;
	mouse.peca2.tipo=-2;
	movimentos=15;
	pontuacao=0;

	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 );
	screen=SDL_SetVideoMode(800,640,16,SDL_SWSURFACE);

	if((Som_de_Troca = Mix_LoadWAV( "sound/low.wav" )) == (NULL))
		return 1;

	Menu();
	
	if (EXIT == true)
	{
		return 0;
	}
	Som_de_quebra=Mix_LoadWAV( "sound/explosao.wav" );

	music = Mix_LoadMUS( "sound/musicaintro.mp3" );
  	Mix_PlayMusic( music, -1 );
	
	int combina=0;
	do
	{
	PreencheMatriz(peca);
	combina=Combina(peca);
	}while(combina<6);


	fundo=IMG_Load("images/fundo1.jpg");
	SDL_BlitSurface(fundo,NULL,screen,NULL);

	radar=IMG_Load("images/radar.png");
	SDL_Rect Radarrect={200,50};
	SDL_BlitSurface(radar,NULL,screen,&Radarrect);

	img = IMG_Load("images/kame.png");
	SDL_Rect rect = { 70, 335, 0, 0 };

	SDL_BlitSurface(img, NULL, screen, &rect);

	TTF_Init();
	TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 26);
	SDL_Rect textLocation = { 450, 20, 0, 0 };
	SDL_Color foregroundColor = { 0, 0, 0 };
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Score:", foregroundColor);
	SDL_BlitSurface(textSurface,NULL,screen,&textLocation);
	BlitaPontos ();
	SDL_Flip(screen);
	
	const int FPS = 10;
	Uint32 start;

	while(movimentos > 0 && combina>0)
	{
		//SDL_BlitSurface(radar,NULL,screen,&Radarrect);
		combina=Combina(peca);
		blitar(peca);
		start=SDL_GetTicks();
		VerificaMatriz(peca);
		while(SDL_PollEvent(&event))
		{
			if( event.type==SDL_QUIT)
			{
				Mix_PauseMusic();
				SDL_Quit();
				return 0;
			}
			if( event.type == SDL_MOUSEBUTTONDOWN )
		    {

  	    	if( event.button.button == SDL_BUTTON_LEFT )
  	   		{
						mouse.x=event.button.x;
						mouse.y=event.button.y;
						if(event.button.x > rect.x && event.button.x < rect.x+rect.w && event.button.y > rect.y && event.button.y < rect.y+rect.h && pontuacao>10)
							dica(peca);
						else
						VerificaTroca(peca,&mouse);						
				}
			}
		}

		SDL_Flip(screen);
		if(1000/FPS > SDL_GetTicks()-start)
			SDL_Delay(1000/FPS);
	}
	VerificaMatriz(peca);
	SDL_Flip(screen);
	Efeito_Recorde(&recordes);
	//recorde(&recordes,pontuacao);

	Mix_PauseMusic();
	main(0,"abc");

	SDL_Quit();

	return 0;
}

void PreencheMatriz(PECA peca[8][8])
{
	int i,j;

	srand(time(NULL));
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			peca[i][j].tipo=rand() %npecas;

			if((i==0 || i==1) && (j>=2 && j<8))//verificar primeira e segunda linhas
			{
				while((peca[i][j].tipo==peca[i][j-1].tipo) && (peca[i][j].tipo==peca[i][j-2].tipo))
				{
					peca[i][j].tipo=rand() %npecas;
				}
			}
			else// if(i>1 && j>1 && j<8)//verificar outras
			{			
				while( ( (peca[i][j].tipo==peca[i][j-1].tipo) && (peca[i][j].tipo==peca[i][j-2].tipo) ) || ( (peca[i][j].tipo==peca[i-1][j].tipo) && (peca[i][j].tipo==peca[i-2][j].tipo) ) )
				{
					peca[i][j].tipo=rand() %npecas;
				}
			}
		}
	}

}


void blitar(PECA peca[8][8])
{
	int i,j;
	SDL_Surface *image;

	int	x0=400,y0=200;

	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			if(peca[i][j].tipo==0)
				image=IMG_Load("images/1.png");
			else if(peca[i][j].tipo==1)
				image=IMG_Load("images/2.png");
			else if(peca[i][j].tipo==2)
				image=IMG_Load("images/3.png");
			else if(peca[i][j].tipo==3)
				image=IMG_Load("images/4.png");
			else if(peca[i][j].tipo==4)
				image=IMG_Load("images/5.png");
			else if(peca[i][j].tipo==5)
				image=IMG_Load("images/6.png");
			else if(peca[i][j].tipo==6)
				image=IMG_Load("images/7.png");
			else
				image=NULL;
			//SDL_SetAlpha(*screen, SDL_SRCALPHA, 30);

			peca[i][j].rect.x=x0 + j*40;
			peca[i][j].rect.y=y0 + i*40;
			SDL_BlitSurface(image,NULL,screen,&peca[i][j].rect);
		/*SDL_SetColorKey(image, SDL_SRCCOLORKEY, SDL_MapRGB(image->format, 0, 0, 0));
		image=SDL_DisplayFormat(image);*/

		}

		//peca[i][j].rect.y=y0+i*peca[i][j].rect.h;
//		peca[i][j].rect.x=0;

	}
}

int ColideMouse(SDL_Rect r1, MOUSE mouse)
{
	if((mouse.x>r1.x)&&(mouse.x<r1.x+r1.w))
	{
		if((mouse.y>r1.y)&&(mouse.y<r1.y+r1.h))
		{
			return 1;
		}
	}
	else
	{
		return 0;
	}
}
void VerificaTroca(PECA peca[8][8],MOUSE *mouse)
{
	int i,j;
	int	break2=false;

	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			if(ColideMouse(peca[i][j].rect,*mouse)==1)
			{
				if(mouse->peca1.tipo==-2)
				{
					mouse->peca1=peca[i][j];
					mouse->i1=i;
					mouse->j1=j;
					break2=true;
					break;
				}
				else
				{
					mouse->peca2=peca[i][j];
					mouse->i2=i;
					mouse->j2=j;
					break2=true;
					Troca(mouse,peca);

					break;
				}
			}
		}
		if(break2==true)
			break;
	}
	
}

void Troca(MOUSE *mouse,PECA peca[8][8])
{
	
	int Lorigem,Ldestino,Corigem,Cdestino;
	int i,j,quebrar;
	int intermediario;

	Lorigem=mouse->i1;
	Ldestino=mouse->i2;
	Corigem=mouse->j1;
	Cdestino=mouse->j2;

	if( ( ((Ldestino==Lorigem+1) || (Ldestino==Lorigem-1)) && (Corigem==Cdestino) ) || ( ((Cdestino==Corigem+1) || (Cdestino==Corigem-1)) && (Lorigem==Ldestino) )) 
	{	
		Efeito_Troca(peca,mouse);

		intermediario					=	peca[Lorigem][Corigem].tipo;
		peca[Lorigem][Corigem].tipo		=	peca[Ldestino][Cdestino].tipo;	
		peca[Ldestino][Cdestino].tipo	=	intermediario;
		Mix_PlayChannel( -1, Som_de_Troca, 0 );
		blitar(peca);
		SDL_Flip(screen);
		for(i=0;i<8;i++)
		{			
			for(j=0;j<6;j++)
			{
				if(peca[i][j].tipo==peca[i][j+1].tipo && peca[i][j].tipo==peca[i][j+2].tipo)
				{
					quebrar=1;				
				}	
			}
		}
		for(i=0;i<6;i++)
		{
			for(j=0;j<8;j++)
			{
				if(peca[i][j].tipo==peca[i+1][j].tipo && peca[i][j].tipo==peca[i+2][j].tipo)
					quebrar=1;
			}
		}


		if (quebrar==1)
		{
			movimentos--;
			quebra(peca);			
		}

		else
		{			
			//SDL_Delay(250);
			Efeito_Troca(peca,mouse);

			intermediario					=	peca[Lorigem][Corigem].tipo;
			peca[Lorigem][Corigem].tipo		=	peca[Ldestino][Cdestino].tipo;	
			peca[Ldestino][Cdestino].tipo	=	intermediario;

			Mix_PlayChannel( -1, Som_de_Troca, 0 );
			blitar(peca);
			SDL_Flip(screen);
		}

	}

	else
	{
		printf("\nMovimento invalido\n");
		//movimentos--;
	}
	mouse->peca1.tipo=-2;
}

int Combina(PECA peca[8][8])
{
	int i,j;
	int combinacoes=0;

	for(i=0;i<8;i++)
	{			
		for(j=0;j<5;j++)
		{
			if((peca[i][j].tipo==peca[i][j+2].tipo) && (peca[i][j].tipo==peca[i][j+3].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i][j+1].tipo) && (peca[i][j].tipo==peca[i][j+3].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i][j+1].tipo) && (peca[i][j].tipo==peca[i+1][j+2].tipo) && (i < 7))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j+1].tipo) && (peca[i][j].tipo==peca[i+1][j+2].tipo) && (i < 7))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j+1].tipo) && (peca[i][j].tipo==peca[i+1][j+2].tipo) && (i < 7))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i-1][j+1].tipo) && (peca[i][j].tipo==peca[i-1][j+2].tipo) && (i > 0))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j+1].tipo) && (peca[i][j].tipo==peca[i][j+2].tipo) && (i < 7))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i-1][j+1].tipo) && (peca[i][j].tipo==peca[i][j+2].tipo) && (i > 0))
				combinacoes+=1;


		}
	}

	for(i=0;i<5;i++)
	{			
		for(j=0;j<8;j++)
		{
			if((peca[i][j].tipo==peca[i+2][j].tipo && peca[i][j].tipo==peca[i+3][j].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j].tipo && peca[i][j].tipo==peca[i+3][j].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j-1].tipo && peca[i][j].tipo==peca[i+2][j].tipo) && (j > 0))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j+1].tipo && peca[i][j].tipo==peca[i+2][j].tipo) && (j < 7))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j+1].tipo && peca[i][j].tipo==peca[i+2][j+1].tipo) && (j < 7))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j-1].tipo && peca[i][j].tipo==peca[i+2][j-1].tipo) && (j > 0))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j].tipo && peca[i][j].tipo==peca[i+2][j-1].tipo) && (j > 0))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j].tipo && peca[i][j].tipo==peca[i+2][j+1].tipo) && (j < 7))
				combinacoes+=1;
		}
	}
	return combinacoes;
}


void quebra(PECA peca[8][8])
{
	int	i,j,k,cont;
	int posicoes[2][30];	
	int pcol=0;
	int a,b;

	for(i=0;i<30;i++)
	{
		posicoes[0][i]=-2;
		posicoes[1][i]=-2;				
	}

	for(i=0;i<8;i++)
	{
		for(j=0;j<6;j++)
		{
			if((peca[i][j].tipo==peca[i][j+1].tipo)&&(peca[i][j].tipo==peca[i][j+2].tipo))
			{
				k=peca[i][j].tipo;
				cont=j;
				while((peca[i][cont].tipo==k)&&(cont<8))
				{
					a=i;
					b=cont;
					posicoes[0][pcol]=a;
					posicoes[1][pcol]=b;					
					cont++;
					pcol++;
				}
			}
		}
	}

	for(i=0;i<6;i++)
	{
		for(j=0;j<8;j++)
		{
			if((peca[i][j].tipo==peca[i+1][j].tipo)&&(peca[i][j].tipo==peca[i+2][j].tipo))
			{
				k=peca[i][j].tipo;
				cont=i;
				while((peca[cont][j].tipo==k)&&(cont<8))
				{					
					a=cont;
					b=j;
					posicoes[0][pcol]=a;
					posicoes[1][pcol]=b;
					cont++;
					pcol++;
				}
			}
		}
	}	

	i=0;	
	for(j=0;posicoes[i][j]!=-2;j++)
	{
		if (j>29)
			break;

			a=posicoes[i][j];//linha
			b=posicoes[i+1][j];//coluna

			peca[a][b].tipo=-1;			
	}
	blitar(peca);
	Explode(peca);
	Mix_PlayChannel( -1, Som_de_quebra, 0 );
	//SDL_Flip(screen);
	//SDL_Delay(1000);
	EfeitoCai(peca);
	Score(peca);
	sobe(peca);
}


void sobe(PECA peca[8][8])
{
	int i,j,k,intermediario,UltimoSorteado=9,random;

	srand(time(NULL));

	for(k=0;k<8;k++)
	{
		if(peca[0][k].tipo==-1)
		{
			random=rand()%npecas;
			while(random==UltimoSorteado)
				random=rand()%npecas;

			UltimoSorteado=random;
			peca[0][k].tipo=random;
		}
	}
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			if((peca[i][j].tipo==-1) && (i!=0))
			{
				for(k=i;k>0;k--)
				{
					intermediario		=	peca[k-1][j].tipo;
					peca[k-1][j].tipo	=	peca[k][j].tipo;
					peca[k][j].tipo		=	intermediario;					
				}				
				for(k=0;k<8;k++)
				{
					if(peca[0][k].tipo==-1)
					{
						random=rand()%npecas;
						while(random==UltimoSorteado)
							random=rand()%npecas;
	
						UltimoSorteado=random;
						peca[0][k].tipo=random;
					}
				}
			}
		}
	}
	VerificaMatriz(peca);
}


void Score(PECA peca[8][8])
{
	int cont=0,i,j;

	for(i=0;i<8;i++)
	{
			
		for(j=0;j<8;j++)
		{
			if(peca[i][j].tipo==-1)
			{
				cont++;
			}	
		}
	}

	if(cont==3)
		pontuacao+=cont*5;
	else if((cont>3)&&(cont<=6)) //combo
		pontuacao+=cont*7;
	else if(cont>6)//super combo
		pontuacao+=cont*10;	

	BlitaPontos ();
	blitar(peca);
	SDL_Flip(screen);

}

void VerificaMatriz(PECA peca[8][8])
{
	int i,j;
	int quebrar=false;
	
	for(i=0;i<8;i++)
	{			
		for(j=0;j<6;j++)
		{
			if(peca[i][j].tipo==peca[i][j+1].tipo && peca[i][j].tipo==peca[i][j+2].tipo)
			{
				quebrar=true;
			}	
		}
	}
	for(i=0;i<6;i++)
	{			
		for(j=0;j<8;j++)
		{
			if(peca[i][j].tipo==peca[i+1][j].tipo && peca[i][j].tipo==peca[i+2][j].tipo)
				quebrar=true;
		}
	}
	if(quebrar==true)
		quebra(peca);
comb = Combina(peca);
}


void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination)
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, NULL, destination, &offset );
}


void Menu()
{

	SDL_Surface *imagem=NULL;
	SDL_Surface *temp=NULL;
	SDL_Surface *fundo=NULL;
	SDL_Surface *logo=NULL;
	SDL_Rect src,dest;
	SDL_Rect Ofundo,Dfundo;
	SDL_Event event;
	int running=true;
	int incx,incy;
	TTF_Init();
	int mousex =0, mousey =0;
	int mousedentro1;
	int mousedentro2;
	Uint32 start;
	Mix_Music *menu_music = NULL;

	int vxfundo=1,desloc=1;

	menu_music = Mix_LoadMUS( "sound/musicamenu.mp3" );

	//if( Mix_PlayingMusic() == 0 )
    	Mix_PlayMusic( menu_music, -1 );

	TTF_Font* font = TTF_OpenFont("rec.ttf", 48);
	SDL_Color foregroundColor = { 0, 255, 0 };
	//SDL_Color backgroundColor = { 0, 120, 255 };

	SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Play", foregroundColor);
	SDL_Rect textLocation = { 320, 350, 0, 0 };

	SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, "Records", foregroundColor);
	SDL_Rect textLocation2 = { 295, 400, 0, 0 };

	temp=IMG_Load("images/ceu.jpg");

	logo=IMG_Load("images/logo.png");
	SDL_Rect logorect={90,450};

	fundo=SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);	
	Ofundo.x = 0;
	Ofundo.y = 0;
	Ofundo.w = fundo->w;
	Ofundo.h = fundo->h;

	Dfundo.x = 0;
	Dfundo.y = 0;
	Dfundo.w = fundo->w;
	Dfundo.h = fundo->h;

	SDL_BlitSurface(fundo, &Ofundo, screen, &Dfundo);

	temp=IMG_Load("goku.bmp");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	imagem=SDL_DisplayFormat(temp);
	src.x=0;
	src.y=0;
	src.w=imagem->w;
	src.h=imagem->h;

	dest.x=30;
	dest.y=30;
	dest.w=imagem->w;
	dest.h=imagem->h;

	incx=2;
	incy=1;
	const int FPS=60;

	int volta=false;
	while(running)
	{	
		start=SDL_GetTicks();
		while(SDL_PollEvent(&event))
		{
			if( event.type==SDL_QUIT)
			{
				running=false;
				EXIT = true;
			}
			if( event.type == SDL_MOUSEMOTION )
			{
				mousex = event.motion.x;
				mousey = event.motion.y;
				if( (mousex > textLocation.x) && (mousex < (textLocation.x + textLocation.w)) && (mousey > textLocation.y) && (mousey < (textLocation.y + textLocation.h)) )
				{
					if(mousedentro1==false)
					{
						Mix_PlayChannel( -1, Som_de_Troca, 0 );
						mousedentro1=true;
					}

					SDL_Color foregroundColor = { 0, 255, 255 };
					textSurface = TTF_RenderText_Solid(font, "Play", foregroundColor);
				}
				else
				{
					SDL_Color foregroundColor = { 0, 255, 0 };
					textSurface = TTF_RenderText_Solid(font, "Play", foregroundColor);
					mousedentro1=false;
				}
				if( (mousex > textLocation2.x) && (mousex < (textLocation2.x + textLocation2.w)) && (mousey > textLocation2.y) && (mousey < (textLocation2.y + textLocation2.h)) )
				{
					if(mousedentro2==false)
					{
						Mix_PlayChannel( -1, Som_de_Troca, 0 );
						mousedentro2=true;
					}
					SDL_Color foregroundColor = { 0, 255, 255 };
					textSurface2 = TTF_RenderText_Solid(font, "Records", foregroundColor);
				}
				else
				{
					SDL_Color foregroundColor = { 0,255, 0 };
					textSurface2 = TTF_RenderText_Solid(font, "Records", foregroundColor);
					mousedentro2=false;
				}
			}

			if( event.type == SDL_MOUSEBUTTONDOWN )
		 	{
				mousex = event.button.x;
				mousey = event.button.y;

			if( (mousex > textLocation.x) && (mousex < (textLocation.x + textLocation.w)) && (mousey > textLocation.y) && (mousey < (textLocation.y + textLocation.h)) )
				{
					running=false;
				}


			if( (mousex > textLocation2.x) && (mousex < (textLocation2.x + textLocation2.w)) && (mousey > textLocation2.y) && (mousey < (textLocation2.y + textLocation2.h)) )
				{	
					ApresentaRecordes();
				}

			}
		}
	
		if ( ((dest.x + dest.w + incx) <= screen->w) && (dest.x + incx >= 0) )
		{ 
			dest.x += incx;
		}
		 else
		{     
			incx=-incx;
		}
		
		if (((dest.y + dest.h + incy) <= screen->h) && (dest.y + incy >= 0))
		{
			dest.y += incy;
		}	
		else
		{	
			incy=-incy;
		}

		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

		//SDL_Flip(screen);
		if( Dfundo.x <= -fundo->w )
    {
      vxfundo = 0;
    }		
		vxfundo-=desloc;
		if(vxfundo==-200)
		{
			desloc*=-1;
			volta=true;
		}
		if(vxfundo==-1 && volta == true)
		{
			desloc*=-1;
		}

    apply_surface( vxfundo, 0, fundo, screen);
		apply_surface( vxfundo + fundo->w, 0, fundo, screen);

		SDL_BlitSurface(imagem,&src,screen,&dest);
		SDL_BlitSurface(logo,NULL,screen,&logorect);
		SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
		SDL_BlitSurface(textSurface2, NULL, screen, &textLocation2);
		
		
		SDL_Flip(screen);
		if(1000/FPS > SDL_GetTicks()-start)
			SDL_Delay(1000/FPS);
	}
	SDL_FreeSurface(textSurface);
	TTF_CloseFont(font);
	TTF_Quit();
	Mix_PauseMusic();

	SDL_FreeSurface(fundo);
}

void dica(PECA matriz[8][8])
{
	int i,j, k;
	int v[2][150];
	int dicas, combinacoes=0;
	srand (time(NULL));

	for (k=0; k<150; k++)
	{
		v[0][k] = -3;
		v[1][k] = -3;
	}

	k=0;
	for(i=0;i<8;i++)
	{
		for(j=0;j<5;j++)
		{
			if((matriz[i][j].tipo==matriz[i][j+2].tipo) && (matriz[i][j].tipo==matriz[i][j+3].tipo)) //1
      		{
				combinacoes+=1;
				v[0][k] = i;
				v[1][k] = j;
				k++;
			}
			if((matriz[i][j].tipo==matriz[i][j+1].tipo) && (matriz[i][j].tipo==matriz[i][j+3].tipo))  //2
			{
				combinacoes+=1;
				v[0][k] = i;
				v[1][k] = j+3;
				k++;
			}
			if((matriz[i][j].tipo==matriz[i][j+1].tipo) && (matriz[i][j].tipo==matriz[i+1][j+2].tipo) && (i < 7)) //3
     		{
        		combinacoes+=1;
        		v[0][k] = i+1;
				v[1][k] = j+2;
				k++;
            }
			if((matriz[i][j].tipo==matriz[i+1][j+1].tipo) && (matriz[i][j].tipo==matriz[i+1][j+2].tipo) && (i < 7))  //4
     		{
        		combinacoes+=1;
        		v[0][k] = i;
				v[1][k] = j;
				k++;
      		}

			if((matriz[i][j].tipo==matriz[i+1][j+1].tipo) && (matriz[i][j].tipo==matriz[i+1][j+2].tipo) && (i < 7))  //5
      		{
        		combinacoes+=1;
        		v[0][k] = i;
				v[1][k] = j;
				k++;
      		}

			if((matriz[i][j].tipo==matriz[i-1][j+1].tipo) && (matriz[i][j].tipo==matriz[i-1][j+2].tipo) && (i > 0)) //6
     		{
        		combinacoes+=1;
        		v[0][k] = i;
				v[1][k] = j;
				k++;
     		}

			if((matriz[i][j].tipo==matriz[i+1][j+1].tipo) && (matriz[i][j].tipo==matriz[i][j+2].tipo) && (i < 7))  //7
     		{
        		combinacoes+=1;
        		v[0][k] = i+1;
				v[1][k] = j+1;
				k++;
     		}

			if((matriz[i][j].tipo==matriz[i-1][j+1].tipo) && (matriz[i][j].tipo==matriz[i][j+2].tipo) && (i > 0))  //8
     		{
        		combinacoes+=1;
        		v[0][k] = i-1;
				v[1][k] = j+1;
				k++;
     		}
		}
	}

	for(i=0;i<5;i++)
	{
		for(j=0;j<8;j++)
		{
			if((matriz[i][j].tipo==matriz[i+2][j].tipo && matriz[i][j].tipo==matriz[i+3][j].tipo))  //1
      		{
				combinacoes+=1;
				v[0][k] = i;
				v[1][k] = j;
				k++;
     		}
			if((matriz[i][j].tipo==matriz[i+1][j].tipo && matriz[i][j].tipo==matriz[i+3][j].tipo))  //2
     		{
        		combinacoes+=1;
        		v[0][k] = i+3;
				v[1][k] = j;
				k++;
      		}
			if((matriz[i][j].tipo==matriz[i+1][j-1].tipo && matriz[i][j].tipo==matriz[i+2][j].tipo) && (j > 0))  //3
     		{
       			combinacoes+=1;
        		v[0][k] = i+1;
				v[1][k] = j-1;
				k++;
     		}
			if((matriz[i][j].tipo==matriz[i+1][j+1].tipo && matriz[i][j].tipo==matriz[i+2][j].tipo) && (j < 7))  //4
     		{
        		combinacoes+=1;
        		v[0][k] = i+1;
				v[1][k] = j+1;
				k++;
      		}
			if((matriz[i][j].tipo==matriz[i+1][j+1].tipo && matriz[i][j].tipo==matriz[i+2][j+1].tipo) && (j < 7))  //5
      		{
       			combinacoes+=1;
        		v[0][k] = i;
				v[1][k] = j;
				k++;
     		}
			if((matriz[i][j].tipo==matriz[i+1][j-1].tipo && matriz[i][j].tipo==matriz[i+2][j-1].tipo) && (j > 0))  //6
     		{
		    	combinacoes+=1;
		    	v[0][k] = i;
				v[1][k] = j;
				k++;
      		}
			if((matriz[i][j].tipo==matriz[i+1][j].tipo && matriz[i][j].tipo==matriz[i+2][j-1].tipo) && (j > 0))  //7
      		{
	      		combinacoes+=1;
        		v[0][k] = i+2;
				v[1][k] = j-1;
				k++;
     		}
			if((matriz[i][j].tipo==matriz[i+1][j].tipo && matriz[i][j].tipo==matriz[i+2][j+1].tipo) && (j < 7))  //8
      		{
	      		combinacoes+=1;
        		v[0][k] = i+2;
				v[1][k] = j+1;
				k++;
     		}
		}
	}
	int l=0;
	do
	{
		l = rand () %k;
	} while (l < 0);

	
	printf ("L   %d\n\n", v[0][l]);
	printf ("C   %d\n\n", v[1][l]);
	pontuacao-=10;
	//printf("Score: %d",pontuacao);

	Mix_Chunk *dica=NULL;
	dica = Mix_LoadWAV( "sound/dica.wav" );
	Mix_PlayChannel( -1, dica, 0 );

	for(k=1;k<10;k++)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_Rect rect = { 70, 335, 0, 0 };
		SDL_BlitSurface(img,NULL,screen,&rect);
		TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 26);
		SDL_Rect textLocation = { 400, 50, 0, 0 };
		SDL_Color foregroundColor = { 0, 0, 0 };
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Score:", foregroundColor);
		SDL_BlitSurface(textSurface,NULL,screen,&textLocation);
		BlitaPontos ();
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{		
				if((i!=v[0][l]) || (j != v[1][l]))
				{	
					CarregaImagem(matriz,i,j);
					matriz[i][j].rect.x=400 + j*40;
					matriz[i][j].rect.y=200 + i*40;
					SDL_BlitSurface(joia,NULL,screen,&matriz[i][j].rect);
				}					
			}
		}
			i=v[0][l];
			j=v[1][l];
			matriz[i][j].rect.x=400 + j*40;
			matriz[i][j].rect.y=200 + (i*40) - k;
			CarregaImagem(matriz,i,j);
			SDL_BlitSurface(joia,NULL,screen,&matriz[i][j].rect);
			SDL_Flip(screen);	
	}
	for(k=1;k<10;k++)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_Rect rect = { 70, 335, 0, 0 };
		SDL_BlitSurface(img,NULL,screen,&rect);
		TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 26);
		SDL_Rect textLocation = { 400, 50, 0, 0 };
		SDL_Color foregroundColor = { 0, 0, 0 };
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Score:", foregroundColor);
		SDL_BlitSurface(textSurface,NULL,screen,&textLocation);
		BlitaPontos ();
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{		
				if(i!=v[0][l] || j != v[1][l])
				{	
					CarregaImagem(matriz,i,j);
					matriz[i][j].rect.x=400 + j*40;
					matriz[i][j].rect.y=200 + i*40;
					SDL_BlitSurface(joia,NULL,screen,&matriz[i][j].rect);
				}					
			}
		}
			i=v[0][l];
			j=v[1][l];
			matriz[i][j].rect.x=400 + j*40;
			matriz[i][j].rect.y=190 + (i*40) + k;
			CarregaImagem(matriz,i,j);
			SDL_BlitSurface(joia,NULL,screen,&matriz[i][j].rect);
			SDL_Flip(screen);	
	}


}

void CarregaImagem (PECA matriz[8][8],int i, int j)
{

		if(matriz[i][j].tipo==0)
			joia=IMG_Load("images/1.png");
		else if(matriz[i][j].tipo==1)
			joia=IMG_Load("images/2.png");
		else if(matriz[i][j].tipo==2)
			joia=IMG_Load("images/3.png");
		else if(matriz[i][j].tipo==3)
			joia=IMG_Load("images/4.png");
		else if(matriz[i][j].tipo==4)
			joia=IMG_Load("images/5.png");
		else if(matriz[i][j].tipo==5)
			joia=IMG_Load("images/6.png");
		else if(matriz[i][j].tipo==6)
			joia=IMG_Load("images/7.png");
		else
			joia=NULL;
}

void Efeito_Troca(PECA peca[8][8],MOUSE *mouse)
{
	if(mouse->j1 == mouse->j2) //a peça i1 j1 tem q subir e a outra descer
		Troca_Vertical(peca,mouse);
	if(mouse->i1 == mouse->i2)
		Troca_Horizontal(peca,mouse);
}
void Troca_Vertical(PECA peca[8][8],MOUSE *mouse)
{
	int i,j,k;
	
	for(k=0;k<40;k+=4)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_Rect rect = { 70, 335, 0, 0 };
		SDL_BlitSurface(img,NULL,screen,&rect);
		TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 26);
		SDL_Rect textLocation = { 400, 50, 0, 0 };
		SDL_Color foregroundColor = { 0, 0, 0 };
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Score:", foregroundColor);
		SDL_BlitSurface(textSurface,NULL,screen,&textLocation);
		BlitaPontos ();
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{		
				if( ((i!=mouse->i1) || (j != mouse->j1)) && ((i!=mouse->i2) || (j != mouse->j2)) )
				{	
					CarregaImagem(peca,i,j);
					peca[i][j].rect.x=400 + j*40;
					peca[i][j].rect.y=200 + i*40;
					SDL_BlitSurface(joia,NULL,screen,&peca[i][j].rect);
				}					
			}
		}
			if(mouse->i1 > mouse->i2)
			{
				i=mouse->i1;
				j=mouse->j1;
			}
			else
			{
				i=mouse->i2;
				j=mouse->j2;
			}
			peca[i][j].rect.x=400 + j*40;
			peca[i][j].rect.y=200 + (i*40) - k;
			CarregaImagem(peca,i,j);
			SDL_BlitSurface(joia,NULL,screen,&peca[i][j].rect);

			if(mouse->i1 > mouse->i2)
			{
				i=mouse->i2;
				j=mouse->j2;
			}
			else
			{
				i=mouse->i1;
				j=mouse->j1;
			}
			peca[i][j].rect.x=400 + j*40;
			peca[i][j].rect.y=200 + (i*40) + k;
			CarregaImagem(peca,i,j);
			SDL_BlitSurface(joia,NULL,screen,&peca[i][j].rect);

			SDL_Flip(screen);	
	}
}

void Troca_Horizontal(PECA peca[8][8],MOUSE *mouse)
{
	int i,j,k;
	
	for(k=0;k<40;k+=4)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_Rect rect = { 70, 335, 0, 0 };
		SDL_BlitSurface(img,NULL,screen,&rect);
		TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 26);
		SDL_Rect textLocation = { 400, 50, 0, 0 };
		SDL_Color foregroundColor = { 0, 0, 0 };
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Score:", foregroundColor);
		SDL_BlitSurface(textSurface,NULL,screen,&textLocation);
		BlitaPontos ();
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{		
				if( ((i!=mouse->i1) || (j != mouse->j1)) && ((i!=mouse->i2) || (j != mouse->j2)) )
				{	
					CarregaImagem(peca,i,j);
					peca[i][j].rect.x=400 + j*40;
					peca[i][j].rect.y=200 + i*40;
					SDL_BlitSurface(joia,NULL,screen,&peca[i][j].rect);
				}					
			}
		}
		if(mouse->j1 > mouse->j2)
		{
			i=mouse->i1;
			j=mouse->j1;
		}
		else
		{
			i=mouse->i2;
			j=mouse->j2;
		}
		peca[i][j].rect.x=400 + j*40 - k;
		peca[i][j].rect.y=200 + (i*40);
		CarregaImagem(peca,i,j);
		SDL_BlitSurface(joia,NULL,screen,&peca[i][j].rect);

		if(mouse->j1 > mouse->j2)
		{
			i=mouse->i2;
			j=mouse->j2;
		}
		else
		{
			i=mouse->i1;
			j=mouse->j1;
		}
		peca[i][j].rect.x=400 + j*40 + k;
		peca[i][j].rect.y=200 + (i*40);
		CarregaImagem(peca,i,j);
		SDL_BlitSurface(joia,NULL,screen,&peca[i][j].rect);

		SDL_Flip(screen);	
	}
}
// TRANSFORMA NUMERO EM STRING int num = 123; 	char buf[5]; itoa(num, buf);

void itoa(int n, char s[])
{
	int i, sign;

   	if ((sign = n) < 0) 
      	n = -n;

   	i = 0;
   	do 
	{
       	s[i++] = n % 10 + '0';
  	} while ((n /= 10) > 0);

  	 if (sign < 0)
       	s[i++] = '-';
   	s[i] = '\0';
   	reverse(s);
}
void reverse(char s[])
{
   	int i, j;
   	char c;

  	for (i = 0, j = strlen(s)-1; i<j; i++, j--) 
	{
     	c = s[i];
     	s[i] = s[j];
     	s[j] = c;
   	}
}

void BlitaPontos ()
{
	char pont[10];
	itoa(pontuacao,pont);
	TTF_Init();

	SDL_BlitSurface(fundo,NULL,screen,NULL);
	SDL_Rect rect = { 70, 335, 0, 0 };
	SDL_BlitSurface(img,NULL,screen,&rect);

	TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 26);
	SDL_Rect textLocation = { 400, 50, 0, 0 };
	SDL_Color foregroundColor = { 0, 0, 0 };
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Score:", foregroundColor);
	SDL_BlitSurface(textSurface,NULL,screen,&textLocation);
	SDL_Rect PontLocation = { 500, 50, 0, 0 };
	SDL_Color PontColor = { 0, 0, 0 };
	textSurface = TTF_RenderText_Solid(font, pont, PontColor);
	SDL_BlitSurface(textSurface,NULL,screen,&PontLocation);

	//BlitaTempo();
}
void Efeito_Recorde(R_ARRAY *recordes)
{

	SDL_Surface *text=NULL;
	SDL_Rect rect={350,320};
	TTF_Font *font = NULL;
	SDL_Color textColor = { 0, 0, 0 };

	SDL_Surface *text2=NULL;
	SDL_Rect rect2={350,220};
	SDL_Color textColor2 = { 0, 0, 255 };


	SDL_Event event;
	

  font = TTF_OpenFont( "ARIAL.TTF", 36 );

	SDL_EnableUNICODE( SDL_ENABLE );
	fundo=IMG_Load("images/fundo2.jpg");

	int quit=false;
	char temp[20]={' '};
	char a;
	int i=0;
	

	SDL_BlitSurface(fundo,NULL,screen,NULL);
	SDL_Flip(screen);
	while(quit==false)
	{
    while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_KEYDOWN )
				{
				    //Keep a copy of the current version of the string
				 		if(event.key.keysym.unicode== (Uint16)'\r' )//no sdl o \r representa o \n
						{
							quit=true;
						}
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
                quit = true;
            }
				}
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		text2 = TTF_RenderText_Solid( font,"Novo Recorde!!", textColor2 );
		SDL_BlitSurface(text2,NULL,screen,&rect2);
		SDL_BlitSurface(text,NULL,screen,&rect);
		SDL_Delay(1000/60);
		SDL_Flip(screen);
		}

	//temp[i]='\0';
	recorde(recordes,pontuacao,temp);
}


void recorde(R_ARRAY *recordes,int valor,char *nome)
{
    char *nome_arquivo = "recordes.bin";
    FILE *p_arquivo;
    R_ARRAY array;
    array.n = 0;

    if((p_arquivo = fopen(nome_arquivo, "r")) != NULL)
    {
        fread(&array, sizeof(R_ARRAY), 1, p_arquivo);
        fclose(p_arquivo);
    }

    imprime(&array);

    RECORDES rec1;
    
    printf("Nome do Recordista: ");
		strcpy (rec1.nome,nome);
    //fgets(rec1.nome, MAX_NOME, stdin);
		
    //rec1.nome[strlen(rec1.nome) - 1] = '\0';

	rec1.pontuacao = pontuacao;

    insere(&array, rec1);
    
    imprime(&array);

    if((p_arquivo = fopen(nome_arquivo, "w")) == NULL)
    {
        perror("fopen:");
       // return 1;
    }

    fwrite(&array, sizeof(R_ARRAY), 1, p_arquivo);
    fclose(p_arquivo);

    //return 0;
}

RECORDES elemento(R_ARRAY *array, int posicao)
{
	return array->vetor[posicao];
}

int tamanho(R_ARRAY *array)
{
	return array->n;
}

void insere(R_ARRAY *array, RECORDES recorde)
{
    if(tamanho(array) < MAX_ARRAY)
    {
        array->vetor[tamanho(array)] = recorde;
        array->n += 1;
    }
    else
    {
        RECORDES menor_recorde;
        menor_recorde = elemento(array, MAX_ARRAY - 1);
        
        if(menor_recorde.pontuacao >= recorde.pontuacao)
        {
            return;
        }
        else
        {
            remove_posicao(array, MAX_ARRAY - 1);
            insere(array, recorde);
            return;
        }
    }
    
    sort(array);
}

void remove_posicao(R_ARRAY *array, int posicao)
{
	if(tamanho(array) == 0)
	{
		return;
	}
	else if(tamanho(array) == 1)
	{
		array->n -= 1;
		return;
	}
	else
	{
		array->n -= 1;
		array->vetor[posicao] = array->vetor[tamanho(array)];
	}
    
    sort(array);
}

void imprime(R_ARRAY *array)
{
	int i;
    RECORDES recorde;

    printf("------------------------------\n");
	printf("Recordes:\n");

	for(i = 0; i < tamanho(array); i++)
	{
        recorde = elemento(array, i);
        printf("%d - Score: %d - Player: %s \n", i+1, recorde.pontuacao, recorde.nome);
	}

	printf("------------------------------\n");
}

void sort(R_ARRAY *array)
{
	bubble_sort(array->vetor, array->n);
}

void bubble_sort(RECORDES list[], int n)
{
	int i, j;
    RECORDES swap;

	for(i = 0 ; i < ( n - 1 ); i++)
	{
		for(j = 0 ; j < n - i - 1; j++)
		{
			if(list[j].pontuacao < list[j+1].pontuacao)
			{ 
				swap = list[j];
				list[j] = list[j+1];
				list[j+1] = swap;
			}
		}
	}
}

void BlitaTempo ()
{	
	char segundos[4];

	
	//BlitaPontos();

	TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 26);
	SDL_Rect textLocation = { 100, 50, 0, 0 };
	SDL_Color foregroundColor = { 0, 0, 0 };
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Tempo:", foregroundColor);


	SDL_BlitSurface(textSurface,NULL,screen,&textLocation);
	SDL_Rect PontLocation = { 200, 50, 0, 0 };
	SDL_Color PontColor = { 0, 0, 0 };
	textSurface = TTF_RenderText_Solid(font, segundos, PontColor);
	SDL_BlitSurface(textSurface,NULL,screen,&PontLocation);


}

void Explode(PECA peca[8][8])
{
	int i,j,k;

	SDL_Surface *exp[11];
	SDL_Surface *temp=NULL;

	temp=IMG_Load("images/1b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[0]=SDL_DisplayFormat(temp);

	temp=IMG_Load("images/2b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[1]=SDL_DisplayFormat(temp);

	temp=IMG_Load("images/3b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[2]=SDL_DisplayFormat(temp);

	temp=IMG_Load("images/4b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[3]=SDL_DisplayFormat(temp);

	temp=IMG_Load("images/5b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[4]=SDL_DisplayFormat(temp);

	temp=IMG_Load("images/6b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[5]=SDL_DisplayFormat(temp);

	temp=IMG_Load("images/7b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[6]=SDL_DisplayFormat(temp);

	temp=IMG_Load("images/8b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[7]=SDL_DisplayFormat(temp);

	temp=IMG_Load("images/9b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[8]=SDL_DisplayFormat(temp);

	temp=IMG_Load("images/9b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[9]=SDL_DisplayFormat(temp);

	temp=IMG_Load("images/11b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[10]=SDL_DisplayFormat(temp);

	for(k=0;k<12;k++)
	{
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{
				if(peca[i][j].tipo==-1)
					SDL_BlitSurface(exp[k],NULL,screen,&peca[i][j].rect);
			}
		}
		SDL_Delay(100);
		SDL_Flip(screen);
	}
	SDL_Delay(100);
}

void EfeitoCai(PECA matriz[8][8])
{
	int i,j,k,a;
	int m[8][8]= {0};


	for(k=1;k<=10;k++)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_Rect rect = { 70, 335, 0, 0 };
		SDL_BlitSurface(img,NULL,screen,&rect);
		TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 26);
		SDL_Rect textLocation = { 400, 50, 0, 0 };
		SDL_Color foregroundColor = { 0, 0, 0 };
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Score:", foregroundColor);
		SDL_BlitSurface(textSurface,NULL,screen,&textLocation);
		BlitaPontos ();

		for (i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{
				if(matriz[i][j].tipo == -1)
				{
					for(a=i-1;a>=0;a--)
					{
						Desce(matriz,a,j);
						m[a][j] = 1;
					}
				}
			}
		}
		for (i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{
				if (m[i][j] == 0)
				{
					CarregaImagem(matriz,i,j);
					matriz[i][j].rect.x = 400 + j*40;
					matriz[i][j].rect.y = 200 + i*40;
					SDL_BlitSurface(joia,NULL,screen,&matriz[i][j].rect);
				}
			}
		}
		SDL_Flip(screen);
	}
}

void Desce(PECA matriz[8][8], int i, int j)
{		
		matriz[i][j].rect.x=400 + j*40;
		matriz[i][j].rect.y+=4;
		CarregaImagem(matriz,i,j);
		SDL_BlitSurface(joia,NULL,screen,&matriz[i][j].rect);	
}

void ApresentaRecordes()
{
	R_ARRAY array;
	FILE *rec;
	array.n=0;

	rec=fopen("recordes.bin","r");
	fread(&array,sizeof(R_ARRAY),1,rec);

	TTF_Init();
	TTF_Font *font=TTF_OpenFont("rec.ttf",36);
	SDL_Surface *nome;
	SDL_Surface *pontos;
	SDL_Color color={255,0,0};
	SDL_Rect posicao={10,100};

	int i;
	char pontuacao[4];

	fundo=IMG_Load("images/dragon.jpg");

	SDL_BlitSurface(fundo,NULL,screen,NULL);

	for(i=0;i<array.n;i++)
	{
		posicao.x=80;
		nome = TTF_RenderText_Solid(font, array.vetor[i].nome , color);
		SDL_BlitSurface(nome,NULL,screen,&posicao);

		itoa(array.vetor[i].pontuacao,pontuacao);
		posicao.x+=200;	
		pontos=TTF_RenderText_Solid(font,pontuacao, color);
		SDL_BlitSurface(pontos,NULL,screen,&posicao);

		posicao.y+=50;
	}
	SDL_Flip(screen);
	SDL_Delay(10000);

}
