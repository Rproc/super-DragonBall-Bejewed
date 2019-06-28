#include"SDL/SDL.h"
#include"SDL/SDL_image.h"
#include"stdio.h"
#include "time.h"
#include "stdlib.h"

#define true 1
#define false 0

int npecas =7;
int movimentos=15;
int pontuacao=0;

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


void PreencheMatriz(PECA peca[8][8]);
void blitar(PECA peca[8][8],SDL_Surface **screen);
int ColideMouse(SDL_Rect r1, MOUSE mouse);
void VerificaTroca(PECA peca[8][8],MOUSE *mouse);
void Troca(MOUSE *mouse,PECA peca[8][8]);
int Combina(PECA peca[8][8]);
void quebra(PECA peca[8][8]);
void sobe(PECA peca[8][8]);
void VerificaMatriz(PECA peca[8][8]);
void Score(PECA peca[8][8]);

int main(int argc,char **argv)
{
	SDL_Surface *screen;	
	PECA peca[8][8];
	int running = true;
	SDL_Event event;
	MOUSE mouse;

	mouse.peca1.tipo=-2;
	mouse.peca2.tipo=-2;

	SDL_Init(SDL_INIT_EVERYTHING);
	screen=SDL_SetVideoMode(800,640,16,SDL_SWSURFACE);

	int combina=0;

	do
	{
		PreencheMatriz(peca);
		combina=Combina(peca);
	}while(combina<6);

	while(running)
	{
		blitar(peca,&screen);
		SDL_Flip(screen);
		VerificaMatriz(peca);
		while(SDL_PollEvent(&event))
		{
			if( event.type==SDL_QUIT)
				running=false;

			if( event.type == SDL_MOUSEBUTTONDOWN )
		    {
				if( event.button.button == SDL_BUTTON_LEFT )
    	    	{
					printf("click! %d  %d\n",event.button.x,event.button.y);
					mouse.x=event.button.x;
					mouse.y=event.button.y;
					VerificaTroca(peca,&mouse);
				}
			}
		}
	}	
	

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


void blitar(PECA peca[8][8],SDL_Surface **screen)
{
	int i,j;
	//SDL_Rect src;
	//src.x=0;
	//src.y=0;
	SDL_Surface *image;
	//peca.image=IMG_Load("joia1.tga");
	//peca[0][0].rect.x=0;
	//peca[0][0].rect.y=0;
	int	x0=100,y0=100;

	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			if(peca[i][j].tipo==0)
				image=IMG_Load("joia1.tga");
			else if(peca[i][j].tipo==1)
				image=IMG_Load("joia2.tga");
			else if(peca[i][j].tipo==2)
				image=IMG_Load("joia3.tga");
			else if(peca[i][j].tipo==3)
				image=IMG_Load("joia4.tga");
			else if(peca[i][j].tipo==4)
				image=IMG_Load("joia5.tga");
			else if(peca[i][j].tipo==5)
				image=IMG_Load("joia6.tga");
			else if(peca[i][j].tipo==6)
				image=IMG_Load("joia7.tga");
		
			//SDL_SetAlpha(*screen, SDL_SRCALPHA, 30);

			peca[i][j].rect.x=x0+j*28;
			peca[i][j].rect.y=y0+i*28;
			SDL_BlitSurface(image,NULL,*screen,&peca[i][j].rect);
			
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
					printf("%d %d\n",mouse->peca1.tipo,mouse->peca2.tipo);
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
		intermediario				=	peca[Lorigem][Corigem].tipo;
		peca[Lorigem][Corigem].tipo	=	peca[Ldestino][Cdestino].tipo;	
		peca[Ldestino][Cdestino].tipo	=	intermediario;

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
//			printf("chamar a quebra\n");
			quebra(peca);

		else
		{
			intermediario				=	peca[Lorigem][Corigem].tipo;
			peca[Lorigem][Corigem].tipo	=	peca[Ldestino][Cdestino].tipo;	
			peca[Ldestino][Cdestino].tipo	=	intermediario;
			movimentos--;
		}

	}

	else
	{
		printf("\nMovimento invalido\n");
		movimentos--;
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
			if((peca[i][j].tipo==peca[i+1][j+1].tipo && peca[i][j].tipo==peca[i+1][j].tipo))
				combinacoes+=1;
			if((peca[i][j].tipo==peca[i+1][j-1].tipo && peca[i][j].tipo==peca[i+1][j].tipo))
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
					intermediario=peca[k-1][j].tipo;
					peca[k-1][j].tipo=peca[k][j].tipo;
					peca[k][j].tipo=intermediario;
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


	for(i=0;i<8;i++)
	{
		for(j=0;j<6;j++)
		{
			if(((peca[i][j].tipo==peca[i][j+1].tipo)&&(peca[i][j].tipo==peca[i][j+2].tipo)) || ((peca[i][j].tipo==peca[i+1][j].tipo)&&(peca[i][j].tipo==peca[i+2][j].tipo)))
			{
				quebrar=1;
			}
		}
	}
	if(quebrar==1)	
		quebra(peca);
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


printf("%d",pontuacao);
}

void VerificaMatriz(PECA peca[8][8])
{
	int i,j;

	
	for(i=0;i<8;i++)
	{			
		for(j=0;j<6;j++)
		{
			if(peca[i][j].tipo==peca[i][j+1].tipo && peca[i][j].tipo==peca[i][j+2].tipo)
			{
				quebra(peca);
			}	
		}
	}
	for(i=0;i<6;i++)
	{			
		for(j=0;j<8;j++)
		{
			if(peca[i][j].tipo==peca[i+1][j].tipo && peca[i][j].tipo==peca[i+2][j].tipo)
				quebra(peca);
		}
	}
}
