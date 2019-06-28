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
			if((matriz[i][j].tipo==matriz[i+1][j+1].tipo && matriz[i][j].tipo==matriz[i+1][j].tipo))
      		{
       			combinacoes+=1;
        		v[0][k] = i+1;
				v[1][k] = j+1;
				k++;
     		}
			if((matriz[i][j].tipo==matriz[i+1][j-1].tipo && matriz[i][j].tipo==matriz[i+1][j].tipo))
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

	v[0][l];
	v[1][l];
	
	printf ("L   %d\n\n", v[0][l]);
	printf ("C   %d\n\n", v[1][l]);

	pontuacao -= 10;

	printf ("Score  %d\n\n", pontuacao);
	//return dicas;
}

