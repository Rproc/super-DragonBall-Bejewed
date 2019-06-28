#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"

#include "stdio.h"
#include "time.h"
#include "string.h"
#include "stdlib.h"

#define true 1
#define false 0

typedef struct
{
	int pontuacao;
	char nome[100];
}RECORDES;

typedef struct{
	RECORDES v[10];
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
SDL_Surface *img=NULL;
Mix_Chunk *Som_de_Troca=NULL;
Mix_Chunk *Som_de_quebra=NULL;
Mix_Music *music=NULL;


void InsereRecorde(R_ARRAY *recordes,int valor);
void ZeraRec(R_ARRAY *recordes);

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

void Efeito_Troca(PECA peca[8][8],MOUSE *mouse);
void Troca_Vertical(PECA peca[8][8],MOUSE *mouse);
void Troca_Horizontal(PECA peca[8][8],MOUSE *mouse);


int main(int argc,char **argv)
{

	PECA peca[8][8];
//	int running = true;
	SDL_Event event;
	MOUSE mouse;
	R_ARRAY recordes;

	mouse.peca1.tipo=-2;
	mouse.peca2.tipo=-2;

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

	img = IMG_Load("images/mrpopo.png");
	SDL_Rect rect = { 70, 435, 0, 0 };

	SDL_BlitSurface(img, NULL, screen, &rect);

	TTF_Init();
	TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 26);
	SDL_Rect textLocation = { 400, 50, 0, 0 };
	SDL_Color foregroundColor = { 0, 0, 0 };
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Score:", foregroundColor);
	SDL_BlitSurface(textSurface,NULL,screen,&textLocation);
	BlitaPontos ();
	SDL_Flip(screen);

	while(movimentos > 0 && combina>0)
	{
		combina=Combina(peca);
		blitar(peca);
		SDL_Flip(screen);
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
	}	
	//InsereRecorde(&recordes,pontuacao);

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
	//SDL_Rect src;
	//src.x=0;
	//src.y=0;
	SDL_Surface *image;
	//peca.image=IMG_Load("joia1.tga");
	//peca[0][0].rect.x=0;
	//peca[0][0].rect.y=0;
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
			if((peca[i][j].tipo==peca[i+1][j+1].tipo) && (peca[i][j].tipo==peca[i+1][j+2].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i][j+1].tipo) && (peca[i][j].tipo==peca[i+1][j+2].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j+1].tipo) && (peca[i][j].tipo==peca[i][j+2].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j-1].tipo) && (peca[i][j].tipo==peca[i+1][j+1].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i][j+1].tipo) && (peca[i][j].tipo==peca[i-1][j+2].tipo))
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
			if((peca[i][j].tipo==peca[i+1][j+1].tipo && peca[i][j].tipo==peca[i+2][j+1].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j].tipo && peca[i][j].tipo==peca[i+2][j+1].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j+1].tipo && peca[i][j].tipo==peca[i+2][j].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j-1].tipo && peca[i][j].tipo==peca[i+2][j-1].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j].tipo && peca[i][j].tipo==peca[i-1][j+1].tipo))
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
	Mix_PlayChannel( -1, Som_de_quebra, 0 );
	SDL_Flip(screen);
	SDL_Delay(1000);
	Score(peca);
	sobe(peca);
}


void sobe(PECA peca[8][8])
{
	int i,j,k,intermediario,UltimoSorteado=9,random;
	int quebrar=0;

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


	printf("\nScore: %d\n",pontuacao);
	BlitaPontos ();
	SDL_Flip(screen);
	//if (comb == 0)
	//	running = true;

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

void Menu()
{

	SDL_Surface *imagem=NULL;
	SDL_Surface *temp=NULL;
	SDL_Surface *fundo=NULL;
	SDL_Rect src,dest;
	SDL_Rect Ofundo,Dfundo;
	SDL_Event event;
	int running=true;
	int incx,incy,quica=0;
	TTF_Init();
	int mousex =0, mousey =0;
	int mousedentro1;
	int mousedentro2;

	Mix_Music *menu_music = NULL;

	menu_music = Mix_LoadMUS( "sound/chala.mp3" );

	if( Mix_PlayingMusic() == 0 )
    	Mix_PlayMusic( menu_music, -1 );

	TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 48);
	SDL_Color foregroundColor = { 255, 0, 0 };
	//SDL_Color backgroundColor = { 0, 120, 255 };

	SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Play", foregroundColor);

	SDL_Rect textLocation = { 320, 300, 0, 0 };

	SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, "Options", foregroundColor);
	SDL_Rect textLocation2 = { 290, 450, 0, 0 };

	temp=IMG_Load("images/fundo1.jpg");

	fundo=SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);	
	Ofundo.x = 0;
	Ofundo.y = 0;
	Ofundo.w = fundo->w;
	Ofundo.h = fundo->h;

	Dfundo.x = 00;		
	Dfundo.y = 00;
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
	while(running)
	{	
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
					SDL_Color foregroundColor = { 255, 0, 0 };
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
					textSurface2 = TTF_RenderText_Solid(font, "Options", foregroundColor);
				}
				else
				{
					SDL_Color foregroundColor = { 255, 0, 0 };
					textSurface2 = TTF_RenderText_Solid(font, "Options", foregroundColor);
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
		SDL_BlitSurface(fundo,&Ofundo,screen,&Dfundo);
		SDL_BlitSurface(imagem,&src,screen,&dest);
		SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
		SDL_BlitSurface(textSurface2, NULL, screen, &textLocation2);
		SDL_Flip(screen);
	}
	SDL_FreeSurface(textSurface);
	TTF_CloseFont(font);
	TTF_Quit();
	Mix_PauseMusic();

	SDL_FreeSurface(fundo);
}

/*
void InsereRecorde(R_ARRAY *recordes,int valor)
{
	int i,j;
	FILE *rec;
	char *FileName ="rec.bin";
	ZeraRec(recordes);

	rec=fopen(FileName,"r");

	//if(rec=fopen(FileName,"r")==NULL) 
		//rec=fopen(FileName,"w");

	i=0;

	while(!feof(rec))
	{
		fscanf(rec,"%s %d",recordes->v[i].nome,&recordes->v[i].pontuacao);
		i++;
	}
	fclose(rec);

	for(i=0;i<10;i++)
	{
		if(valor>recordes->v[i].pontuacao)
		{
			for(j=9;j>i;j--)
			{
				recordes->v[j].pontuacao=recordes->v[j-1].pontuacao;
				strcpy(recordes->v[j].nome,recordes->v[j-1].nome);
			}
			recordes->v[j].pontuacao=valor;
			gets(recordes->v[j].nome);
			break;
		}
	}

	rec=fopen(FileName,"w");
	// gravar no arquivo os dados das variaveis	
	for(i=0;recordes->v[i].pontuacao>0;i++)
	{
		fwrite(&recordes->v[i].pontuacao, sizeof (int), 1, rec);
		for (j=0; recordes -> v[i].nome[j] != '\0';j++)
			fwrite (&recordes->v[i].nome[j], sizeof (char), 1, rec);
	}
	for(i=0;recordes->v[i].pontuacao>0;i++)

	fclose(rec);
}

void ZeraRec(R_ARRAY *recordes)
{
	int i;

	for(i=0;i<10;i++)
	{
		recordes->v[i].nome[0]='\0';
		recordes->v[i].pontuacao=0;
	}
}
*/

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
			if((matriz[i][j].tipo==matriz[i][j+2].tipo) && (matriz[i][j].tipo==matriz[i][j+3].tipo))
      		{
				combinacoes+=1;
				v[0][k] = i;
				v[1][k] = j;
				k++;
			}
			if((matriz[i][j].tipo==matriz[i][j+1].tipo) && (matriz[i][j].tipo==matriz[i][j+3].tipo))
			{
				combinacoes+=1;
				v[0][k] = i;
				v[1][k] = j+3;
				k++;
			}
			if((matriz[i][j].tipo==matriz[i+1][j+1].tipo) && (matriz[i][j].tipo==matriz[i+1][j+2].tipo))
     		{
        		combinacoes+=1;
        		v[0][k] = i;
				v[1][k] = j;
				k++;
            }
			if((matriz[i][j].tipo==matriz[i][j+1].tipo) && (matriz[i][j].tipo==matriz[i+1][j+2].tipo))
     		{
        		combinacoes+=1;
        		v[0][k] = i+1;
				v[1][k] = j+2;
				k++;
      		}

			if((matriz[i][j].tipo==matriz[i+1][j+1].tipo) && (matriz[i][j].tipo==matriz[i][j+2].tipo))
      		{
        		combinacoes+=1;
        		v[0][k] = i+1;
				v[1][k] = j+1;
				k++;
      		}

			if((matriz[i][j].tipo==matriz[i+1][j-1].tipo) && (matriz[i][j].tipo==matriz[i+1][j+1].tipo))
     		{
        		combinacoes+=1;
        		v[0][k] = i;
				v[1][k] = j;
				k++;
     		}
			if((matriz[i][j].tipo==matriz[i][j+1].tipo) && (matriz[i][j].tipo==matriz[i-1][j+2].tipo))
     		{
        		combinacoes+=1;
        		v[0][k] = i-1;
				v[1][k] = j+2;
				k++;
     		}
		}
	}

	for(i=0;i<5;i++)
	{
		for(j=0;j<8;j++)
		{
			if((matriz[i][j].tipo==matriz[i+2][j].tipo && matriz[i][j].tipo==matriz[i+3][j].tipo))
      		{
				combinacoes+=1;
				v[0][k] = i;
				v[1][k] = j;
				k++;
     		}
			if((matriz[i][j].tipo==matriz[i+1][j].tipo && matriz[i][j].tipo==matriz[i+3][j].tipo))
     		{
        		combinacoes+=1;
        		v[0][k] = i+3;
				v[1][k] = j;
				k++;
      		}
			if((matriz[i][j].tipo==matriz[i+1][j+1].tipo && matriz[i][j].tipo==matriz[i+2][j+1].tipo))
     		{
       			combinacoes+=1;
        		v[0][k] = i;
				v[1][k] = j;
				k++;
     		}
			if((matriz[i][j].tipo==matriz[i+1][j].tipo && matriz[i][j].tipo==matriz[i+2][j+1].tipo))
     		{
        		combinacoes+=1;
        		v[0][k] = i+2;
				v[1][k] = j+1;
				k++;
      		}
			if((matriz[i][j].tipo==matriz[i+1][j+1].tipo && matriz[i][j].tipo==matriz[i+2][j].tipo))
      		{
       			combinacoes+=1;
        		v[0][k] = i+1;
				v[1][k] = j+1;
				k++;
     		}
			if((matriz[i][j].tipo==matriz[i+1][j-1].tipo && matriz[i][j].tipo==matriz[i+2][j-1].tipo))
     		{
		    	combinacoes+=1;
		    	v[0][k] = i;
				v[1][k] = j;
				k++;
      		}
			if((matriz[i][j].tipo==matriz[i+1][j].tipo && matriz[i][j].tipo==matriz[i-1][j+1].tipo))
      		{
	      		combinacoes+=1;
        		v[0][k] = i-1;
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
	printf("Score: %d",pontuacao);

	Mix_Chunk *dica=NULL;
	dica = Mix_LoadWAV( "sound/dica.wav" );
	Mix_PlayChannel( -1, dica, 0 );

	for(k=1;k<10;k++)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_Rect rect = { 70, 435, 0, 0 };
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
		SDL_Rect rect = { 70, 435, 0, 0 };
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
	
	for(k=0;k<40;k+=2)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_Rect rect = { 70, 435, 0, 0 };
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
	
	for(k=0;k<40;k+=2)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_Rect rect = { 70, 435, 0, 0 };
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
   do {
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
	SDL_Rect rect = { 70, 435, 0, 0 };
	SDL_BlitSurface(img,NULL,screen,&rect);

	TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 26);
	SDL_Rect textLocation = { 400, 50, 0, 0 };
	SDL_Color foregroundColor = { 0, 0, 0 };
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Score:", foregroundColor);
	SDL_BlitSurface(textSurface,NULL,screen,&textLocation);
	SDL_Rect PontLocation = { 500, 100, 0, 0 };
	SDL_Color PontColor = { 0, 0, 0 };
	textSurface = TTF_RenderText_Solid(font, pont, PontColor);
	SDL_BlitSurface(textSurface,NULL,screen,&PontLocation);
	

}
