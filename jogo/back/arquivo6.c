#include <stdio.h>

int main(void)
{
	int i;
	int inum;

	char c[20];
	FILE *pa;
	char *nome = "rec.bin";
	
	if((pa = fopen(nome, "r")) == NULL)
	{
		perror("fopen:");
		return 1;
	}

	fread(&inum, sizeof(int), 1, pa);

	for (i=0; c[i] != '\0'; i++)
		fread(&c[i], sizeof(char), 1, pa);
	
	printf("Pont: %d\nName: %s\n", inum, c);
	
	fclose(pa);

	return 0;
}
