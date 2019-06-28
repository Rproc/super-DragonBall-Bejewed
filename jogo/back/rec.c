#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARRAY 10
#define MAX_NOME 50

typedef struct
{
	int pontuacao;
	char nome[50];
}RECORDES;

typedef struct{
	RECORDES vetor[10];
	int n;
}R_ARRAY;

RECORDES elemento(R_ARRAY *, int);
int tamanho(R_ARRAY *);
void insere(R_ARRAY *, RECORDES);

void remove_posicao(R_ARRAY *, int);
void imprime(R_ARRAY *);
void sort(R_ARRAY *);
void bubble_sort(RECORDES *, int);

void recorde(R_ARRAY *recordes,int valor)
{
    char *nome_arquivo = "recordes.dat";
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
    fgets(rec1.nome, MAX_NOME, stdin);

    rec1.nome[strlen(rec1.nome) - 1] = '\0';

	rec1.pontuacao = pontuacao;

    insere(&array, rec1);
    
    imprime(&array);

    if((p_arquivo = fopen(nome_arquivo, "w")) == NULL)
    {
        perror("fopen:");
        return 1;
    }

    fwrite(&array, sizeof(R_ARRAY), 1, p_arquivo);
    fclose(p_arquivo);

    return 0;
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

