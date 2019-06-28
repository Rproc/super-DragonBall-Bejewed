/*
	Trabalho de computação
	Alunos: Douglas Castro, Douglas Moraes e Renan Procopio
*/

//Bibliotecas usadas
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#include "stdio.h"
#include "time.h"
#include "string.h"
#include "stdlib.h"

//Defines
#define MAX_ARRAY 10
#define MAX_NOME 50
#define true 1
#define false 0

//Structs Usadas
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
	int i1,j1;
	int i2,j2;
}MOUSE;


//Seção de Variaveis globais
int Exit = false;
int jogar=false;
int npecas =7;
int pontuacao=0;

SDL_Surface *screen;
SDL_Surface *joia[8];
SDL_Surface *pergaminho;
SDL_Surface *fundo=NULL;
SDL_Surface *fundoRec[4];
SDL_Surface *img;
SDL_Surface *exp[10];
SDL_Surface *gokutempo;
SDL_Surface *barratempo;
SDL_Rect gokurect={0,1};
SDL_Surface *veg;
SDL_Surface *goku;
SDL_Rect gokuDest={200,300};
SDL_Rect vegDest={500,300};

Mix_Music *music=NULL;
Mix_Music *menu_music=NULL;
Mix_Chunk *Som_de_Troca;
Mix_Chunk *Som_de_quebra;
Mix_Chunk *Dica;
Mix_Chunk *Fim;
Mix_Chunk *Combo1;
Mix_Chunk *Combo2;

TTF_Font *font_Menu=NULL;
TTF_Font *font_Pontos=NULL;
TTF_Font *font_Recordes=NULL;
TTF_Font *font_Fim=NULL;
TTF_Font *font_Rec=NULL;
RECORDES elemento(R_ARRAY *, int);

//funcoes sem efeitos
void recorde(R_ARRAY *recordes,int valor,char *nome);
int tamanho(R_ARRAY *);
void insere(R_ARRAY *, RECORDES);
void remove_posicao(R_ARRAY *, int);
void sort(R_ARRAY *);
void bubble_sort(RECORDES *, int);
void PreencheMatriz(PECA peca[8][8]);
int ColideMouse(SDL_Rect r1, MOUSE mouse);
void VerificaTroca(PECA peca[8][8],MOUSE *mouse);
void Troca(MOUSE *mouse,PECA peca[8][8]);
int Combina(PECA peca[8][8]);
void quebra(PECA peca[8][8]);
void sobe(PECA peca[8][8]);
void VerificaMatriz(PECA peca[8][8]);
void Score(PECA peca[8][8]);
void itoa(int n, char s[]);
void reverse(char s[]);
void CarregaMusica();
void CarregaTTF();

//funcoes com efeitos
int Menu();
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination);
void blitar(PECA peca[8][8]);
int EscolhePeca(PECA peca[8][8],int i,int j);
void dica(PECA matriz[8][8]);
void CarregaImagem();
void BlitaPontos();
void Efeito_Troca(PECA peca[8][8],MOUSE *mouse);
void Troca_Vertical(PECA peca[8][8],MOUSE *mouse);
void Troca_Horizontal(PECA peca[8][8],MOUSE *mouse);
void CarregaExplosao();
void Explode(PECA peca[8][8]);
void EfeitoCai(PECA peca[8][8]);
void Desce(PECA matriz[8][8], int i, int j);
void Efeito_Recorde(R_ARRAY *recordes);
void ApresentaRecordes();
void Jogar();
void Pause();
int BlitaPoses();
int Confronto();
int Golpes();
int GokuContraAtaque();
void VoltaPosicao();
int Confronto2();
int Porradero();
int Recua();
void intro();



int main(int argc, char *argv[])
{

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 );
	screen=SDL_SetVideoMode(800,640,16,SDL_SWSURFACE);

	intro();

	CarregaImagem();
	CarregaExplosao();
	CarregaTTF();
	fundo=IMG_Load("images/fundo1.jpg");
	fundoRec[0]=IMG_Load("images/recorde1.png");
	fundoRec[1]=IMG_Load("images/recorde2.png");
	fundoRec[2]=IMG_Load("images/recorde3.jpg");
	fundoRec[3]=IMG_Load("images/recorde4.png");
	img = IMG_Load("images/kame.png");
	barratempo =IMG_Load("images/barratempo.png");
	gokutempo=IMG_Load("images/goku40.png");
	pergaminho=IMG_Load("images/pergaminho.png");
	CarregaMusica();

	do
	{
		Menu();
		if(jogar==true)
			Jogar();
		jogar=false;
	}	while(Exit==false);

	TTF_Quit();
	SDL_FreeSurface(screen);
	SDL_Quit();

	return 0;
}


void Jogar()
{
	PECA peca[8][8];
	pontuacao=0;
	gokurect.x=0;
	gokurect.y=15;

	SDL_Event event;
	MOUSE mouse;
	mouse.peca1.tipo=-2;
	mouse.peca2.tipo=-2;

 	Mix_PlayMusic( music, -1 );

	int combina=0;
	do
	{
		PreencheMatriz(peca);
		combina=Combina(peca);
	}while(combina<6);

	SDL_BlitSurface(fundo,NULL,screen,NULL);

	SDL_Rect rect = { 70, 335, 0, 0 };
	SDL_BlitSurface(img, NULL, screen, &rect);
	SDL_Flip(screen);

	int gokucont=0;

	SDL_Color Cor_Pause={0,0,0};
	SDL_Rect PauseRect={160,240};
	SDL_Surface *pausetxt=TTF_RenderText_Solid(font_Menu,"Pause",Cor_Pause);
	
	blitar(peca);
	BlitaPontos();
	const int FPS = 10;
	while(combina>0 && (gokurect.x+gokurect.w < screen->w) && Exit == false)
	{
		int t1 = SDL_GetTicks();

		combina=Combina(peca);		

		VerificaMatriz(peca);
		while(SDL_PollEvent(&event))
		{
			if( event.type==SDL_QUIT)
			{
				Mix_PauseMusic();
				Exit=true;
				return 0;
			}
		  if( event.type == SDL_MOUSEMOTION )
		  {
				if((event.motion.x>400 && event.motion.x<720)&&(event.motion.y>200 && event.motion.y<520))
				{
					int x, y;
					x = (event.motion.x-400)/40;
			    y = (event.motion.y-200)/40;

					printf("Coluna %d\n",x);
					printf("Linha %d\n",y);
				}
			}

			if( event.type == SDL_MOUSEBUTTONDOWN )
		  {
  	    	if( event.button.button == SDL_BUTTON_LEFT )
  	   		{
						mouse.x=event.button.x;
						mouse.y=event.button.y;
						if(event.button.x > rect.x && event.button.x < rect.x+rect.w && event.button.y > rect.y && event.button.y < rect.y+rect.h && pontuacao>10)
							dica(peca);
						else if(event.button.x > PauseRect.x && event.button.x < PauseRect.x+PauseRect.w && event.button.y > PauseRect.y && event.button.y < PauseRect.y+PauseRect.h)
						{
							Pause();
							SDL_BlitSurface(fundo,NULL,screen,NULL);
							blitar(peca);
							SDL_BlitSurface(img,NULL,screen,&rect);
							BlitaPontos();
							SDL_BlitSurface(pausetxt,NULL,screen,&PauseRect);
							SDL_BlitSurface(barratempo,NULL,screen,NULL);
							SDL_BlitSurface(gokutempo,NULL,screen,&gokurect);					
						}
						else
							VerificaTroca(peca,&mouse);
					}
			}
		}
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_BlitSurface(img,NULL,screen,&rect);
		BlitaPontos();
		blitar(peca);
		SDL_BlitSurface(pausetxt,NULL,screen,&PauseRect);
		SDL_BlitSurface(barratempo,NULL,screen,NULL);
		SDL_BlitSurface(gokutempo,NULL,screen,&gokurect);
		gokucont++;

		if (pontuacao < 2000)
		{
			if(gokucont==4)
			{
				gokurect.x+=1;
				gokucont=0;
			}
		}

		else if (pontuacao >= 2000 && pontuacao <4000)
		{
			if(gokucont==3)
			{
				gokurect.x+=1;
				gokucont=0;
			}
		}

		else if (pontuacao >= 4000 && pontuacao < 8000)
		{
			if(gokucont==2)
			{
				gokurect.x+=1;
				gokucont=0;
			}
		}

		else if (pontuacao >= 9000)
		{
				gokurect.x+=1;
		}

		SDL_Flip( screen );

    int t2 = SDL_GetTicks();
    int wait = t2 - t1;
    wait = (1000/FPS) - wait;

    if(wait > 0)
    {
        SDL_Delay(wait);
    }

		if(gokurect.x+gokurect.w >= screen->w)
		{
			Mix_PauseMusic();
			Mix_PlayChannel( -1, Fim, 0 );
			SDL_Rect textLocation = { 300, 380, 0, 0 };
			SDL_Color foregroundColor = { 255, 10, 50 };
			SDL_Surface* textSurface = TTF_RenderText_Solid(font_Fim, "O Tempo Acabou!", foregroundColor);
			SDL_BlitSurface(fundo,NULL,screen,NULL);
			SDL_BlitSurface(textSurface,NULL,screen,&textLocation);
			SDL_Flip(screen);
			SDL_Delay(2000);
			SDL_FreeSurface(textSurface);
		}

	}
	VerificaMatriz(peca);
	SDL_Flip(screen);

	R_ARRAY recordes;
	Efeito_Recorde(&recordes);

	Mix_PauseMusic();
}


int Menu()
{

	SDL_Surface *imagem=NULL;
	SDL_Surface *temp=NULL;
	SDL_Surface *fundoMenu=NULL;
	SDL_Surface *logo=NULL;
	SDL_Rect dest;
	SDL_Rect Dfundo;
	SDL_Event event;
	int running=true;
	int incx,incy;
	int mousex =0, mousey =0;
	int mousedentro1;
	int mousedentro2;
	int vxfundo=1,desloc=1;


	Mix_PlayMusic( menu_music, -1 );

	SDL_Color foregroundColor = { 0, 255, 0 };

	SDL_Surface* textSurface = TTF_RenderText_Solid(font_Menu, "Play", foregroundColor);
	SDL_Rect textLocation = { 320, 350, 0, 0 };

	SDL_Surface* textSurface2 = TTF_RenderText_Solid(font_Menu, "Records", foregroundColor);
	SDL_Rect textLocation2 = { 295, 400, 0, 0 };

	temp=IMG_Load("images/ceu.jpg");

	logo=IMG_Load("images/logo.png");
	SDL_Rect logorect={90,450};

	fundoMenu=SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	Dfundo.x = 0;
	Dfundo.y = 0;
	Dfundo.w = fundoMenu->w;
	Dfundo.h = fundoMenu->h;

	SDL_BlitSurface(fundoMenu, NULL, screen, &Dfundo);

	temp=IMG_Load("images/goku.bmp");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	imagem=SDL_DisplayFormat(temp);


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
    int t1 = SDL_GetTicks();
		while(SDL_PollEvent(&event))
		{
			if( event.type==SDL_QUIT)
			{
				running=false;
				Exit = true;
				return 0;
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
					textSurface = TTF_RenderText_Solid(font_Menu, "Play", foregroundColor);
				}
				else
				{
					SDL_Color foregroundColor = { 0, 255, 0 };
					textSurface = TTF_RenderText_Solid(font_Menu, "Play", foregroundColor);
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
					textSurface2 = TTF_RenderText_Solid(font_Menu, "Records", foregroundColor);
				}
				else
				{
					SDL_Color foregroundColor = { 0,255, 0 };
					textSurface2 = TTF_RenderText_Solid(font_Menu, "Records", foregroundColor);
					mousedentro2=false;
				}
			}

			if( event.type == SDL_MOUSEBUTTONDOWN )
		 	{
				mousex = event.button.x;
				mousey = event.button.y;

			if( (mousex > textLocation.x) && (mousex < (textLocation.x + textLocation.w)) && (mousey > textLocation.y) && (mousey < (textLocation.y + textLocation.h)) )
				{
					jogar=true;
					return 0;
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
		if( Dfundo.x <= -fundoMenu->w )
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

   		apply_surface( vxfundo, 0, fundoMenu, screen);
		apply_surface( vxfundo + fundoMenu->w, 0, fundoMenu, screen);

		SDL_BlitSurface(imagem,NULL,screen,&dest);
		SDL_BlitSurface(logo,NULL,screen,&logorect);
		SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
		SDL_BlitSurface(textSurface2, NULL, screen, &textLocation2);


		 SDL_Flip( screen );

        int t2 = SDL_GetTicks();
        int wait = t2 - t1;
        wait = (1000/FPS) - wait;

        if(wait > 0)
        {
            SDL_Delay(wait);
        }

	}
	SDL_FreeSurface(textSurface);
	SDL_FreeSurface(textSurface2);
	Mix_PauseMusic();
	SDL_FreeSurface(fundoMenu);
	SDL_FreeSurface(imagem);
	SDL_FreeSurface(temp);
	SDL_FreeSurface(logo);

	return 0;
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

void CarregaImagem()
{

	joia[0]=IMG_Load("images/1.png");
	joia[1]=IMG_Load("images/2.png");
	joia[2]=IMG_Load("images/3.png");
	joia[3]=IMG_Load("images/4.png");
	joia[4]=IMG_Load("images/5.png");
	joia[5]=IMG_Load("images/6.png");
	joia[6]=IMG_Load("images/7.png");
	joia[7]=NULL;

}

int EscolhePeca(PECA peca[8][8],int i,int j)
{
	int k;

	for(k=0;k<7;k++)
		if(peca[i][j].tipo==k)
			return k;
}


void blitar(PECA peca[8][8])
{
	int i=0,j=0;
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			peca[i][j].rect.x=400 + j*40;
			peca[i][j].rect.y=200 + i*40;

			int k=EscolhePeca(peca,i,j);
			SDL_BlitSurface(joia[k],NULL,screen,&peca[i][j].rect);
		}
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

	mouse->peca1.tipo=-2;
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
	int i,j,k,gokucont=0;
	SDL_Rect rect = { 70, 335, 0, 0 };
	SDL_Surface *pausetxt;
	SDL_Color Cor_Pause={0,0,0};
	SDL_Rect PauseRect={160,240};
	pausetxt=TTF_RenderText_Solid(font_Menu,"Pause",Cor_Pause);

	for(k=0;k<=40;k+=4)
	{
		rect.x=70;
		rect.y=335;

		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_BlitSurface(img,NULL,screen,&rect);
		SDL_BlitSurface(barratempo,NULL,screen,NULL);
		SDL_BlitSurface(gokutempo,NULL,screen,&gokurect);
		SDL_BlitSurface(pausetxt,NULL,screen,&PauseRect);
		if(gokucont==3)
		{
			gokurect.x++;
			gokucont=0;
		}
		gokucont++;
		BlitaPontos ();
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{
				if( ((i!=mouse->i1) || (j != mouse->j1)) && ((i!=mouse->i2) || (j != mouse->j2)) )
				{
					int p=EscolhePeca(peca,i,j);

					peca[i][j].rect.x=400 + j*40;
					peca[i][j].rect.y=200 + i*40;
					SDL_BlitSurface(joia[p],NULL,screen,&peca[i][j].rect);
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
			
			int p=EscolhePeca(peca,i,j);

			SDL_BlitSurface(joia[p],NULL,screen,&peca[i][j].rect);

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

			for(p=0;p<7;p++)
				if(p==peca[i][j].tipo)
					break;

			SDL_BlitSurface(joia[p],NULL,screen,&peca[i][j].rect);
			SDL_Flip(screen);
	}

	SDL_BlitSurface(fundo,NULL,screen,NULL);
	SDL_BlitSurface(img,NULL,screen,&rect);
	SDL_BlitSurface(barratempo,NULL,screen,NULL);
	SDL_BlitSurface(gokutempo,NULL,screen,&gokurect);
	SDL_BlitSurface(pausetxt,NULL,screen,&PauseRect);
	SDL_FreeSurface(pausetxt);
	BlitaPontos ();
	blitar(peca);
}

void Troca_Horizontal(PECA peca[8][8],MOUSE *mouse)
{
	int i,j,k,gokucont=0;
	SDL_Surface *pausetxt;
	SDL_Color Cor_Pause={0,0,0};
	SDL_Rect PauseRect={160,240};
	pausetxt=TTF_RenderText_Solid(font_Menu,"Pause",Cor_Pause);

	SDL_BlitSurface(fundo,NULL,screen,NULL);

	SDL_Rect rect={70,335,0,0};
	SDL_BlitSurface(img, NULL, screen, &rect);

	for(k=0;k<=40;k+=4)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_BlitSurface(img,NULL,screen,&rect);
		SDL_BlitSurface(pausetxt,NULL,screen,&PauseRect);
		SDL_BlitSurface(barratempo,NULL,screen,NULL);
		SDL_BlitSurface(gokutempo,NULL,screen,&gokurect);
		gokucont++;
		if(gokucont==3)
		{
			gokurect.x++;
			gokucont=0;
		}
		BlitaPontos ();
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{
				if( ((i!=mouse->i1) || (j != mouse->j1)) && ((i!=mouse->i2) || (j != mouse->j2)) )
				{
					int p=EscolhePeca(peca,i,j);
					peca[i][j].rect.x=400 + j*40;
					peca[i][j].rect.y=200 + i*40;
					SDL_BlitSurface(joia[p],NULL,screen,&peca[i][j].rect);
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
		int p=EscolhePeca(peca,i,j);
		SDL_BlitSurface(joia[p],NULL,screen,&peca[i][j].rect);

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
		for(p=0;p<7;p++)
			if(p==peca[i][j].tipo)
				break;
		SDL_BlitSurface(joia[p],NULL,screen,&peca[i][j].rect);
		SDL_Flip(screen);
	}
	SDL_BlitSurface(fundo,NULL,screen,NULL);
	SDL_BlitSurface(img,NULL,screen,&rect);
	SDL_BlitSurface(pausetxt,NULL,screen,&PauseRect);
	SDL_BlitSurface(barratempo,NULL,screen,NULL);
	SDL_BlitSurface(gokutempo,NULL,screen,&gokurect);
	SDL_FreeSurface(pausetxt);
	BlitaPontos ();
	blitar(peca);
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
	Score(peca);
	Mix_PlayChannel( -1, Som_de_quebra, 0 );
	Explode(peca);
	//SDL_Flip(screen);
	//SDL_Delay(1000);
	//EfeitoCai(peca);
	//Score(peca);
	sobe(peca);
}
void CarregaExplosao()
{

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

	temp=IMG_Load("images/11b.png");
	SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255, 255, 255));
	exp[9]=SDL_DisplayFormat(temp);


	SDL_FreeSurface(temp);
}

void Explode(PECA peca[8][8])
{
	int i,j,k,gokucont=0;
	SDL_Surface *pausetxt;

	for(k=0;k<10;k++)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_Rect rect={70,335};
		SDL_BlitSurface(img,NULL,screen,&rect);
		BlitaPontos();
		SDL_Color Cor_Pause={0,0,0};
		SDL_Rect PauseRect={160,240};
		pausetxt=TTF_RenderText_Solid(font_Menu,"Pause",Cor_Pause);
		SDL_BlitSurface(pausetxt,NULL,screen,&PauseRect);
		SDL_BlitSurface(barratempo,NULL,screen,NULL);
		SDL_BlitSurface(gokutempo,NULL,screen,&gokurect);
		gokucont++;
		if(gokucont==3)
		{
			gokurect.x++;
			gokucont=0;
		}
		
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{
				if(peca[i][j].tipo!=-1)
				{
					int	p=EscolhePeca(peca,i,j);
					SDL_BlitSurface(joia[p],NULL,screen,&peca[i][j].rect);
				}
				if(peca[i][j].tipo==-1)
					SDL_BlitSurface(exp[k],NULL,screen,&peca[i][j].rect);
			}
		}
		SDL_Delay(100);
		SDL_Flip(screen);
	}
	SDL_Delay(100);
	SDL_FreeSurface(pausetxt);
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
		pontuacao+=cont*50;
	else if((cont>3)&&(cont<=6)) //combo
	{
		Mix_PlayChannel( -1, Combo1, 0 );
		pontuacao+=cont*70;
	}
	else if(cont>6)//super combo
	{
		Mix_PlayChannel( -1, Combo2, 0 );
		pontuacao+=cont*100;
	}
}

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
       	s[i] = '-';

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

	SDL_Rect textLocation = { 400, 60, 0, 0 };
	SDL_Color foregroundColor = { 0, 0, 0 };

	SDL_Surface* textSurface = TTF_RenderText_Solid(font_Recordes, "Score:", foregroundColor);

	SDL_BlitSurface(textSurface,NULL,screen,&textLocation);

	SDL_Rect PontLocation = { 500, 60, 0, 0 };
	SDL_Color PontColor = { 0, 0, 0 };

	textSurface = TTF_RenderText_Solid(font_Pontos, pont, PontColor);
	SDL_BlitSurface(textSurface,NULL,screen,&PontLocation);

	SDL_FreeSurface(textSurface);
}


void sobe(PECA peca[8][8])
{
	int i,j,k,intermediario,UltimoSorteado=9,random;

	srand(time(NULL));
	EfeitoCai(peca);
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

	SDL_BlitSurface(fundo,NULL,screen,NULL);

	SDL_Rect rect= { 70, 335, 0, 0 };
	SDL_BlitSurface(img,NULL,screen,&rect);

	BlitaPontos ();
	blitar(peca);
	VerificaMatriz(peca);
}


void EfeitoCai(PECA matriz[8][8])
{
	int i,j,k,a,gokucont=0;
	int m[8][8]= {0};
	SDL_Surface *pausetxt;

	for(k=0;k<10;k++)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_Rect rect = {70, 335, 0, 0 };
		SDL_BlitSurface(img,NULL,screen,&rect);
		SDL_Color Cor_Pause={0,0,0};
		SDL_Rect PauseRect={160,240};
		pausetxt=TTF_RenderText_Solid(font_Menu,"Pause",Cor_Pause);
		SDL_BlitSurface(pausetxt,NULL,screen,&PauseRect);
		SDL_BlitSurface(barratempo,NULL,screen,NULL);
		SDL_BlitSurface(gokutempo,NULL,screen,&gokurect);
		gokucont++;
		if(gokucont==3)
		{
			gokurect.x++;
			gokucont=0;
		}
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
					int p=EscolhePeca(matriz,i,j);

					matriz[i][j].rect.x = 400 + j*40;
					matriz[i][j].rect.y = 200 + i*40;
					SDL_BlitSurface(joia[p],NULL,screen,&matriz[i][j].rect);
				}
			}
		}
		SDL_Flip(screen);
	}
	SDL_FreeSurface(pausetxt);
}

void Desce(PECA matriz[8][8], int i, int j)
{
		matriz[i][j].rect.x=400 + j*40;
		matriz[i][j].rect.y+=4;
		int p=EscolhePeca(matriz,i,j);
		SDL_BlitSurface(joia[p],NULL,screen,&matriz[i][j].rect);

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



void dica(PECA matriz[8][8])
{
	int i,j, k;
	int v[2][150];
	int combinacoes=0;
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

	pontuacao-=100;

	Mix_PlayChannel( -1, Dica, 0 );


	//SDL_BlitSurface(fundo,NULL,screen,NULL);

	SDL_Rect rect = { 70, 335, 0, 0 };
	//SDL_BlitSurface(img, NULL, screen, &rect);
	SDL_Surface *pausetxt;

	for(k=1;k<10;k++)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		//SDL_Rect rect = { 70, 335, 0, 0 };
		SDL_BlitSurface(img,NULL,screen,&rect);
		SDL_Color Cor_Pause={0,0,0};
		SDL_Rect PauseRect={160,240};
		pausetxt=TTF_RenderText_Solid(font_Menu,"Pause",Cor_Pause);
		SDL_BlitSurface(pausetxt,NULL,screen,&PauseRect);
		SDL_BlitSurface(barratempo,NULL,screen,NULL);
		SDL_BlitSurface(gokutempo,NULL,screen,&gokurect);
		gokurect.x++;
		BlitaPontos ();
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{
				if((i!=v[0][l]) || (j != v[1][l]))
				{
					int p=EscolhePeca(matriz,i,j);
					matriz[i][j].rect.x=400 + j*40;
					matriz[i][j].rect.y=200 + i*40;
					SDL_BlitSurface(joia[p],NULL,screen,&matriz[i][j].rect);
				}
			}
		}
			i=v[0][l];
			j=v[1][l];
			matriz[i][j].rect.x=400 + j*40;
			matriz[i][j].rect.y=200 + (i*40) - k;
			int p=EscolhePeca(matriz,i,j);
			SDL_BlitSurface(joia[p],NULL,screen,&matriz[i][j].rect);
			SDL_Flip(screen);
	}
	for(k=1;k<10;k++)
	{
		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_Rect rect = { 70, 335, 0, 0 };
		SDL_Rect PauseRect={160,240};
		SDL_BlitSurface(img,NULL,screen,&rect);
		SDL_BlitSurface(pausetxt,NULL,screen,&PauseRect);
		SDL_BlitSurface(barratempo,NULL,screen,NULL);
		SDL_BlitSurface(gokutempo,NULL,screen,&gokurect);
		BlitaPontos ();
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{
				if(i!=v[0][l] || j != v[1][l])
				{
					int p=EscolhePeca(matriz,i,j);
					matriz[i][j].rect.x=400 + j*40;
					matriz[i][j].rect.y=200 + i*40;
					SDL_BlitSurface(joia[p],NULL,screen,&matriz[i][j].rect);
				}
			}
		}
		i=v[0][l];
		j=v[1][l];
		matriz[i][j].rect.x=400 + j*40;
		matriz[i][j].rect.y=190 + (i*40) + k;
		int p=EscolhePeca(matriz,i,j);
		SDL_BlitSurface(joia[p],NULL,screen,&matriz[i][j].rect);
		SDL_Flip(screen);
		//SDL_FreeSurface(textSurface);
	}
	SDL_Rect PauseRect={160,240};
	SDL_BlitSurface(fundo,NULL,screen,NULL);
	SDL_BlitSurface(img,NULL,screen,&rect);
	SDL_BlitSurface(pausetxt,NULL,screen,&PauseRect);
	SDL_BlitSurface(barratempo,NULL,screen,NULL);
	SDL_BlitSurface(gokutempo,NULL,screen,&gokurect);
	BlitaPontos ();
	blitar(matriz);
}

void Efeito_Recorde(R_ARRAY *recordes)
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

	if((pontuacao > array.vetor[array.n].pontuacao) || (array.n == 0) || (array.n<=10) )
	{

		SDL_Surface *text=NULL;
		SDL_Rect rect={350,320};
		SDL_Color textColor = { 0, 0, 0 };

		SDL_Surface *text2=NULL;
		SDL_Rect rect2={350,220};
		SDL_Color textColor2 = { 0, 0, 255 };


		SDL_Event event;

		SDL_EnableUNICODE( SDL_ENABLE );


		int quit=false;
		char temp[20]={' '};
		int i=0;

		const int FPS = 10;
		//Uint32 start;

		if(pontuacao>9000)
		{
			SDL_BlitSurface(fundoRec[2],NULL,screen,NULL);

			rect2.x=30;
			rect2.y=10;
			textColor2.r=0;
			textColor2.g=0;
			textColor2.b=255;

			rect.x=30;
			rect.y=60;
			textColor.r=0;
			textColor.g=0;
			textColor.b=0;


		}
		else if(pontuacao>8000 && pontuacao <=9000)
		{
			SDL_BlitSurface(fundoRec[3],NULL,screen,NULL);

			rect2.x=30;
			rect2.y=10;
			textColor2.r=255;
			textColor2.g=0;
			textColor2.b=0;

			rect.x=30;
			rect.y=50;
			textColor.r=200;
			textColor.g=0;
			textColor.b=200;

		}
		else if(pontuacao>6000 && pontuacao <=8000)
		{
			SDL_BlitSurface(fundoRec[1],NULL,screen,NULL);

			rect2.x=330;
			rect2.y=360;
			textColor2.r=255;
			textColor2.g=255;
			textColor2.b=0;

			rect.x=300;
			rect.y=420;
			textColor.r=255;
			textColor.g=255;
			textColor.b=255;

		}
		else if(pontuacao<=6000)
		{
			SDL_BlitSurface(fundoRec[0],NULL,screen,NULL);
			rect2.x=310;
			rect2.y=0;

			rect.x=300;
			rect.y=130;
			textColor.r=100;
			textColor.g=255;
			textColor.b=185;
		}

		SDL_Flip(screen);
		while(quit==false)
		{
   			int t1 = SDL_GetTicks();
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
					text = TTF_RenderText_Solid( font_Rec, temp, textColor );
				}
				if( event.type == SDL_QUIT )
				{
					//Quit the program
					quit = true;
				}
			}

		if(pontuacao>9000)
			SDL_BlitSurface(fundoRec[2],NULL,screen,NULL);
		else if(pontuacao>8000 && pontuacao <=9000)
			SDL_BlitSurface(fundoRec[3],NULL,screen,NULL);
		else if(pontuacao>6000 && pontuacao <=8000)
			SDL_BlitSurface(fundoRec[1],NULL,screen,NULL);
		else if(pontuacao<=6000)
			SDL_BlitSurface(fundoRec[0],NULL,screen,NULL);


			text2 = TTF_RenderText_Solid( font_Rec,"Novo Recorde!!", textColor2 );
			SDL_BlitSurface(text2,NULL,screen,&rect2);
			SDL_BlitSurface(text,NULL,screen,&rect);
			SDL_Flip(screen);

	 		int t2 = SDL_GetTicks();
		    int wait = t2 - t1;
		    wait = (1000/FPS) - wait;

		    if(wait > 0)
		    {
		        SDL_Delay(wait);
		    }
		}

		recorde(recordes,pontuacao,temp);

		SDL_FreeSurface(text);
		SDL_FreeSurface(text2);
	}
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

    RECORDES rec1;

	strcpy (rec1.nome,nome);

	rec1.pontuacao = pontuacao;

    insere(&array, rec1);

    if((p_arquivo = fopen(nome_arquivo, "w")) == NULL)
    {
        perror("fopen:");
    }

    fwrite(&array, sizeof(R_ARRAY), 1, p_arquivo);
    fclose(p_arquivo);
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


void ApresentaRecordes()
{
	R_ARRAY array;
	FILE *rec;
	array.n=0;

	rec=fopen("recordes.bin","r");
	fread(&array,sizeof(R_ARRAY),1,rec);


	SDL_Surface *nome;
	SDL_Surface *pontos;
	SDL_Color color={0,0,0};
	SDL_Rect posicao={20,100};

	int i;
	char pontuacao[10];

	
	SDL_Surface *fundo=NULL;
	fundo=IMG_Load("images/dragon.jpg");

	SDL_BlitSurface(fundo,NULL,screen,NULL);
	SDL_Rect pergRect={0,40};
	SDL_BlitSurface(pergaminho,NULL,screen,&pergRect);

	for(i=0;i<array.n;i++)
	{
		posicao.x=40;
		nome = TTF_RenderText_Solid(font_Recordes, array.vetor[i].nome , color);
		SDL_BlitSurface(nome,NULL,screen,&posicao);

		itoa(array.vetor[i].pontuacao,pontuacao);
		posicao.x+=200;
		pontos=TTF_RenderText_Solid(font_Recordes,pontuacao, color);
		SDL_BlitSurface(pontos,NULL,screen,&posicao);

		posicao.y+=50;
	}
	SDL_Flip(screen);
	SDL_Delay(10000);
	SDL_FreeSurface(nome);
	SDL_FreeSurface(pontos);
	SDL_FreeSurface(fundo);
}

void CarregaMusica()
{
	music=Mix_LoadMUS("sound/musicaintro.mp3");
	menu_music=Mix_LoadMUS("sound/chala.mp3");
	Som_de_Troca=Mix_LoadWAV("sound/low.wav");
	Som_de_quebra=Mix_LoadWAV("sound/explosao.wav");
	Dica=Mix_LoadWAV("sound/dica.wav");
	Fim=Mix_LoadWAV("sound/beat.wav");
	Combo1=Mix_LoadWAV("sound/Combo1.wav");
	Combo2=Mix_LoadWAV("sound/Combo2.wav");
}

void CarregaTTF()
{
	font_Menu=TTF_OpenFont("rec.ttf",48);
	font_Pontos=TTF_OpenFont("ARIAL.TTF",26);
	font_Recordes=TTF_OpenFont("rec.ttf",36);
	font_Fim=TTF_OpenFont("rec.ttf",48);
	font_Rec=TTF_OpenFont("recorde.ttf", 36);
}

void Pause()
{
	SDL_Surface *PAUSE=IMG_Load("images/Pause.jpg");
	SDL_Rect pauseimage={50,70};
	SDL_Rect rect1={50,70,40,40};
	SDL_Event event;
	int pausado=true;


	while(pausado==true)
	{
		int FPS=10;
    int t1 = SDL_GetTicks();
		SDL_BlitSurface(PAUSE,NULL,screen,&pauseimage);
		SDL_BlitSurface(joia[0],NULL,screen,&rect1);
		while(SDL_PollEvent(&event))
		{
			if( event.type==SDL_QUIT)
			{
				Exit=true;
				pausado=false;
			}
			if( event.type == SDL_MOUSEBUTTONDOWN )
			  {

			  	if( event.button.button == SDL_BUTTON_LEFT )
			 		{
						if(event.button.x > rect1.x && event.button.x < rect1.x+rect1.w && event.button.y > rect1.y && event.button.y < rect1.y+rect1.h)
						{
							pausado=false;
						}
					}
				}
		}
		SDL_Flip( screen );

    int t2 = SDL_GetTicks();
    int wait = t2 - t1;
    wait = (1000/FPS) - wait;

    if(wait > 0)
    {
        SDL_Delay(wait);
    }

	SDL_Flip(screen);
	}
	SDL_FreeSurface(PAUSE);
}

int BlitaPoses()
{

	int quadros=0;

	int running=1;

	SDL_Rect vegPose[4];

	vegPose[0].x=150;
	vegPose[0].y=175;
	vegPose[0].w=65;
	vegPose[0].h=75;


	vegPose[1].x=220;
	vegPose[1].y=175;
	vegPose[1].w=65;
	vegPose[1].h=75;

	vegPose[2].x=290;
	vegPose[2].y=175;
	vegPose[2].w=65;
	vegPose[2].h=75;

	vegPose[3].x=360;
	vegPose[3].y=175;
	vegPose[3].w=65;
	vegPose[3].h=75;

	SDL_Rect gokuPose[4];

	gokuPose[0].x=470;
	gokuPose[0].y=95;
	gokuPose[0].w=60;
	gokuPose[0].h=80;


	gokuPose[1].x=526;
	gokuPose[1].y=95;
	gokuPose[1].w=60;
	gokuPose[1].h=80;

	gokuPose[2].x=582;
	gokuPose[2].y=95;
	gokuPose[2].w=60;
	gokuPose[2].h=80;

	gokuPose[3].x=640;
	gokuPose[3].y=95;
	gokuPose[3].w=58;
	gokuPose[3].h=80;

	SDL_Event event;

	int VegContPos=0;
	int GokuContPos=0;

		while(running==1)
		{
			int FPS=10;
		  int t1 = SDL_GetTicks();
			while(SDL_PollEvent(&event))
			{
				if( event.type==SDL_QUIT)
					running=0;
			}

			SDL_BlitSurface(fundo,NULL,screen,NULL);

			SDL_BlitSurface(veg,&vegPose[VegContPos],screen,&vegDest);

			if(VegContPos==3)
				VegContPos=0;

			VegContPos++;

			SDL_BlitSurface(goku,&gokuPose[GokuContPos],screen,&gokuDest);
			GokuContPos++;

			if(GokuContPos==3)
				GokuContPos=0;

		  int t2 = SDL_GetTicks();
		  int wait = t2 - t1;
		  wait = (1000/FPS) - wait;
		  if(wait > 0)
		  {
		      SDL_Delay(wait);
		  }
			SDL_Flip(screen);
			quadros++;

			if(quadros==20)
				running = 0;

		}

	return 0;
}

int Confronto()
{

	int running =1;
	SDL_Event event;

	SDL_Rect vegVoa={365,635,55,75};

	SDL_Rect gokuVoa={695,865,75,80};


		while(running==1)
		{
			int FPS=60;
		  int t1 = SDL_GetTicks();
			while(SDL_PollEvent(&event))
			{
				if( event.type==SDL_QUIT)
					running=0;
			}



			SDL_BlitSurface(fundo,NULL,screen,NULL);
			SDL_BlitSurface(veg,&vegVoa,screen,&vegDest);

			SDL_BlitSurface(goku,&gokuVoa,screen,&gokuDest);

			if(vegDest.x>gokuDest.x)
			{
				vegDest.x-=6;
				gokuDest.x+=6;
			}

			if(vegDest.x < gokuDest.x)
			{
				vegDest.x+=4;
				gokuDest.x-=4;
			}

			if(gokuDest.x+gokuDest.w>=vegDest.x+10  &&  gokuDest.x+gokuDest.w <= vegDest.x+vegDest.w)
				running=0;

		  int t2 = SDL_GetTicks();
		  int wait = t2 - t1;
		  wait = (1000/FPS) - wait;
		  if(wait > 0)
		  {
		      SDL_Delay(wait);
		  }
			SDL_Flip(screen);

		}

}

int Golpes()
{


	int running =1;
	SDL_Event event;

	SDL_Rect vegGolpes[4];

	vegGolpes[2].x=120;
	vegGolpes[2].y=250;
	vegGolpes[2].w=75;
	vegGolpes[2].h=85;

	vegGolpes[1].x=200;
	vegGolpes[1].y=250;
	vegGolpes[1].w=45;
	vegGolpes[1].h=85;

	vegGolpes[0].x=240;
	vegGolpes[0].y=250;
	vegGolpes[0].w=45;
	vegGolpes[0].h=85;

	SDL_Rect gokuEsquiva[6];//={350,1280,1000,600};
	gokuEsquiva[5].x=350;
	gokuEsquiva[5].y=1275;
	gokuEsquiva[5].w=40;
	gokuEsquiva[5].h=85;

	gokuEsquiva[4].x=390;
	gokuEsquiva[4].y=1275;
	gokuEsquiva[4].w=38;
	gokuEsquiva[4].h=85;

	gokuEsquiva[3].x=430;
	gokuEsquiva[3].y=1275;
	gokuEsquiva[3].w=38;
	gokuEsquiva[3].h=85;

	gokuEsquiva[2].x=470;
	gokuEsquiva[2].y=1275;
	gokuEsquiva[2].w=38;
	gokuEsquiva[2].h=85;

	gokuEsquiva[2].x=510;
	gokuEsquiva[2].y=1275;
	gokuEsquiva[2].w=38;
	gokuEsquiva[2].h=85;

	gokuEsquiva[1].x=550;
	gokuEsquiva[1].y=1275;
	gokuEsquiva[1].w=38;
	gokuEsquiva[1].h=85;

	gokuEsquiva[0].x=550;
	gokuEsquiva[0].y=1275;
	gokuEsquiva[0].w=38;
	gokuEsquiva[0].h=85;

	int	gokucont=0;

	int VegGolpesCont=0;



		while(running==1)
		{
			int FPS=20;
		  int t1 = SDL_GetTicks();
			while(SDL_PollEvent(&event))
			{
				if( event.type==SDL_QUIT)
					running=0;
			}

			SDL_BlitSurface(fundo,NULL,screen,NULL);
	
			SDL_BlitSurface(veg,&vegGolpes[VegGolpesCont],screen,&vegDest);
			if(VegGolpesCont<2)
				VegGolpesCont++;
	
			SDL_BlitSurface(goku,&gokuEsquiva[gokucont],screen,&gokuDest);
			gokucont++;
			if(gokucont==3)
			{
				gokuDest.x=450;
				gokuDest.y=250;

			}
			if(gokucont==6)
				running=0;

		  int t2 = SDL_GetTicks();
		  int wait = t2 - t1;
		  wait = (1000/FPS) - wait;
		  if(wait > 0)
		  {
		      SDL_Delay(wait);
		  }
			SDL_Flip(screen);

		}

}

int GokuContraAtaque()
{
	int running=1;


	SDL_Rect gokugolpe[3];
	int gokucont=0;

	gokugolpe[2].x=160;
	gokugolpe[2].y=600;
	gokugolpe[2].w=70;
	gokugolpe[2].h=85;

	gokugolpe[1].x=230;
	gokugolpe[1].y=600;
	gokugolpe[1].w=65;
	gokugolpe[1].h=85;

	gokugolpe[0].x=290;
	gokugolpe[0].y=600;
	gokugolpe[0].w=55;
	gokugolpe[0].h=85;


	SDL_Rect vegEsquiva={475,800,60,85};
	SDL_Rect vegVoa={365,635,55,75};
	int CaiPDentro=0;

		SDL_Event event;
		while(running==1)
		{
			int FPS=60;
		  int t1 = SDL_GetTicks();
			while(SDL_PollEvent(&event))
			{
				if( event.type==SDL_QUIT)
					running=0;
			}

			SDL_BlitSurface(fundo,NULL,screen,NULL);

			SDL_BlitSurface(goku,&gokugolpe[gokucont],screen,&gokuDest);
			if(gokucont<2)
				gokucont++;

			SDL_BlitSurface(veg,&vegEsquiva,screen,&vegDest);

			if(vegDest.y > gokuDest.y)
			{
				vegDest.y-=2;
				vegDest.x-=8;
				CaiPDentro=1;
				//printf("%d\n",vegDest.y);
			}

			if(vegDest.y==250)
				running=0;

		  int t2 = SDL_GetTicks();
		  int wait = t2 - t1;
		  wait = (1000/FPS) - wait;
		  if(wait > 0)
		  {
		      SDL_Delay(wait);
		  }
			SDL_Flip(screen);

		}

	VoltaPosicao();
}

void VoltaPosicao()
{
	SDL_Rect vegVoa={365,635,55,75};

	SDL_Rect gokuVoa={695,865,75,80};


	while(gokuDest.y!=300 && vegDest.y!=300)
	{
		int FPS=60;
	  int t1 = SDL_GetTicks();


		if(vegDest.y>300)
			vegDest.y-=2;
		if(vegDest.y<300)
			vegDest.y+=4;

		if(gokuDest.y>300)
			gokuDest.y-=2;
		if(gokuDest.y<300)
			gokuDest.y+=4;

		SDL_BlitSurface(fundo,NULL,screen,NULL);
		SDL_BlitSurface(goku,&gokuVoa,screen,&gokuDest);
		SDL_BlitSurface(veg,&vegVoa,screen,&vegDest);

		  int t2 = SDL_GetTicks();
		  int wait = t2 - t1;
		  wait = (1000/FPS) - wait;
		  if(wait > 0)
		  {
		      SDL_Delay(wait);
		  }
			SDL_Flip(screen);
	}
	
}

int Confronto2()
{

	SDL_Rect vegVoa={425,635,50,75};
	SDL_Rect gokuVoa={165,865,75,80};
	
	SDL_Event event;
	int quadros=0;
	while(gokuDest.x > vegDest.x)
	{	
		int FPS=60;
	  int t1 = SDL_GetTicks();
		while(SDL_PollEvent(&event))
		{
			if( event.type==SDL_QUIT)
				exit(1);
		}

		SDL_BlitSurface(fundo,NULL,screen,NULL);


		SDL_BlitSurface(goku,&gokuVoa,screen,&gokuDest);
		SDL_BlitSurface(veg,&vegVoa,screen,&vegDest);

		if(vegDest.x+vegDest.w <= gokuDest.x)
		{
			gokuDest.x-=6;
			vegDest.x+=6;
		}

		  int t2 = SDL_GetTicks();
		  int wait = t2 - t1;
		  wait = (1000/FPS) - wait;
		  if(wait > 0)
		  {
		      SDL_Delay(wait);
		  }
			SDL_Flip(screen);
			quadros++;
			if(quadros==20)
				return 0;
	}



}

int Porradero()
{

	SDL_Rect gokugolpes[6];
	int gokucont=0;

	gokugolpes[5].x=20;
	gokugolpes[5].y=260;
	gokugolpes[5].w=80;
	gokugolpes[5].h=85;

	gokugolpes[4].x=95;
	gokugolpes[4].y=260;
	gokugolpes[4].w=50;
	gokugolpes[4].h=85;

	gokugolpes[3].x=155;
	gokugolpes[3].y=260;
	gokugolpes[3].w=55;
	gokugolpes[3].h=85;

	gokugolpes[2].x=215;
	gokugolpes[2].y=260;
	gokugolpes[2].w=65;
	gokugolpes[2].h=85;

	gokugolpes[1].x=285;
	gokugolpes[1].y=260;
	gokugolpes[1].w=55;
	gokugolpes[1].h=85;

	gokugolpes[0].x=340;
	gokugolpes[0].y=260;
	gokugolpes[0].w=55;
	gokugolpes[0].h=85;

	//vegeta

	SDL_Rect vegetagolpes[5];
	int vegetacont=0;

	vegetagolpes[0].x=540;
	vegetagolpes[0].y=338;
	vegetagolpes[0].w=55;
	vegetagolpes[0].h=75;

	vegetagolpes[1].x=600;
	vegetagolpes[1].y=338;
	vegetagolpes[1].w=55;
	vegetagolpes[1].h=75;

	vegetagolpes[2].x=655;
	vegetagolpes[2].y=338;
	vegetagolpes[2].w=70;
	vegetagolpes[2].h=75;

	vegetagolpes[3].x=650;
	vegetagolpes[3].y=270;
	vegetagolpes[3].w=75;
	vegetagolpes[3].h=65;

	vegetagolpes[4].x=655;
	vegetagolpes[4].y=338;
	vegetagolpes[4].w=70;
	vegetagolpes[4].h=75;

	SDL_Event event;
	
	
	int quadros=0;
	int i=0;
	int desloc=8;
	while(i<100)
	{	
		int FPS=30;
	  int t1 = SDL_GetTicks();
		while(SDL_PollEvent(&event))
		{
			if( event.type==SDL_QUIT)
				exit(1);
		}

		vegDest.x=gokuDest.x-50;
		vegDest.y=gokuDest.y;

		SDL_BlitSurface(fundo,NULL,screen,NULL);

		SDL_BlitSurface(veg,&vegetagolpes[vegetacont],screen,&vegDest);
		vegetacont++;
		if(vegetacont==5)
			vegetacont=0;
		SDL_BlitSurface(goku,&gokugolpes[gokucont],screen,&gokuDest);
		gokucont++;
		if(gokucont==6)
			gokucont=0;

		  int t2 = SDL_GetTicks();
		  int wait = t2 - t1;
		  wait = (1000/FPS) - wait;
		  if(wait > 0)
		  {
		      SDL_Delay(wait);
		  }
			SDL_Flip(screen);
			quadros++;	
			if(quadros==70)	
			{
				return 0;
			}

		if(quadros>20 && quadros<40)
		{
			gokuDest.x=600;
			gokuDest.y=200;
		}
		else if(quadros>=40 && quadros <60 )
		{
			gokuDest.x=100;
			gokuDest.y=500;
		}
		else if(quadros>=60)
		{
			gokuDest.x=400;
			gokuDest.y=320;
		}

	}

}

int Recua()
{
//goku x 370  goku y 244
//veg x 320 veg y

	SDL_Rect gokuRecuo={285,860,70,85};
	SDL_Rect vegRecuo={477,810,58,75};

	SDL_Event event;
	while(gokuDest.x<500)
	{	
		int FPS=60;
	  int t1 = SDL_GetTicks();
		while(SDL_PollEvent(&event))
		{
			if( event.type==SDL_QUIT)
				exit(1);
		}

		SDL_BlitSurface(fundo,NULL,screen,NULL);

		SDL_BlitSurface(goku,&gokuRecuo,screen,&gokuDest);
		if(gokuDest.x<500)
		{
			gokuDest.x+=8;
			vegDest.x-=8;
		}

		SDL_BlitSurface(veg,&vegRecuo,screen,&vegDest);

	  int t2 = SDL_GetTicks();
	  int wait = t2 - t1;
	  wait = (1000/FPS) - wait;
	  if(wait > 0)
	  {
	      SDL_Delay(wait);
	  }
		SDL_Flip(screen);

	}

	SDL_Rect gokuPose[4];
	int gokucont=0;

	gokuPose[0].x=125;
	gokuPose[0].y=95;
	gokuPose[0].w=55;
	gokuPose[0].h=75;


	gokuPose[1].x=180;
	gokuPose[1].y=95;
	gokuPose[1].w=55;
	gokuPose[1].h=75;

	gokuPose[2].x=240;
	gokuPose[2].y=95;
	gokuPose[2].w=55;
	gokuPose[2].h=75;

	gokuPose[3].x=300;
	gokuPose[3].y=95;
	gokuPose[3].w=55;
	gokuPose[3].h=75;

	SDL_Rect vegPose[4];
	int vegcont=0;

	vegPose[0].x=425;
	vegPose[0].y=175;
	vegPose[0].w=60;
	vegPose[0].h=75;


	vegPose[1].x=490;
	vegPose[1].y=95;
	vegPose[1].w=60;
	vegPose[1].h=80;

	vegPose[2].x=560;
	vegPose[2].y=95;
	vegPose[2].w=60;
	vegPose[2].h=80;

	vegPose[3].x=630;
	vegPose[3].y=95;
	vegPose[3].w=58;
	vegPose[3].h=80;

	int quadros=0;
	while(quadros<20)
	{	
		int FPS=10;
	  int t1 = SDL_GetTicks();	

		SDL_BlitSurface(fundo,NULL,screen,NULL);

		SDL_BlitSurface(goku,&gokuPose[gokucont],screen,&gokuDest);
		gokucont++;

		if(gokucont==4)
			gokucont=0;

		SDL_BlitSurface(veg,&vegPose[vegcont],screen,&vegDest);
		vegcont++;

		if(vegcont==4)
			vegcont=0;

 		int t2 = SDL_GetTicks();
	  int wait = t2 - t1;
	  wait = (1000/FPS) - wait;
	  if(wait > 0)
	  {
	      SDL_Delay(wait);
	  }
		SDL_Flip(screen);
		quadros++;
	}

	SDL_Rect gokuAtack={170,860,70,85};
	SDL_Rect vegAtack={540,810,67,75};
	
	while(vegDest.x+vegDest.w <= gokuDest.x)
	{

		int FPS=60;
	  int t1 = SDL_GetTicks();

		SDL_BlitSurface(fundo,NULL,screen,NULL);	
		SDL_BlitSurface(goku,&gokuAtack,screen,&gokuDest);
		SDL_BlitSurface(veg,&vegAtack,screen,&vegDest);

		vegDest.x+=8;
		gokuDest.x-=8;

 	 int t2 = SDL_GetTicks();
	 int wait = t2 - t1;
	 wait = (1000/FPS) - wait;
	 if(wait > 0)
	 {
	   SDL_Delay(wait);
	 }
		SDL_Flip(screen);
	}

	fundo=IMG_Load("images/fbranco.jpg");
	SDL_BlitSurface(fundo,NULL,screen,NULL);
	SDL_Flip(screen);
	SDL_Delay(1000);
	//SDL_FreeSurface(fundo);
}

void intro()
{
	fundo = IMG_Load("images/ceu.jpg");
	veg = IMG_Load("images/dbz.gif");//vegeta
	goku = IMG_Load("images/dbz2.gif");//goku


	BlitaPoses();
	Confronto();
	Golpes();
	GokuContraAtaque();
	Confronto2();
	Porradero();
	Recua();

	SDL_FreeSurface(veg);
	SDL_FreeSurface(goku);
	SDL_FreeSurface(fundo);

}
