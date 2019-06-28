#include "stdio.h"
#include "string.h"
#include "stdlib.h"


typedef struct
{
	int pontuacao;
	char nome[100];
}RECORDES;

typedef struct{
	RECORDES v[10];
	int n;
}R_ARRAY;


void InsereRecorde(R_ARRAY *recordes,int valor);
void ZeraRec(R_ARRAY *recordes);

int main()
{	
	R_ARRAY recordes;	

	InsereRecorde(&recordes,1000);
	InsereRecorde(&recordes,1500);
	InsereRecorde(&recordes,1555);	

	return 0;
}


void InsereRecorde(R_ARRAY *recordes,int valor)
{
	int i,j;
	FILE *rec;
	char *FileName ="rec.txt";
	ZeraRec(recordes);

	rec=fopen(FileName,"r");
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
		fprintf(rec,"%s\n%d\n",recordes->v[i].nome,recordes->v[i].pontuacao);

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
