#include "radix_tree_file_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		fprintf(stderr, "main: enter the rdt file and a japanese word\n");
		return 4;
	}
	FILE *fp = fopen(argv[1], "rb");
	size_t data_size;
	char *lookup = (char *)radix_tree_file_lookup(fp, (uint8_t *)argv[2], strlen(argv[2]), &data_size);
	if(lookup == NULL)
	{
		printf("Not found\n");
		fclose(fp);
		return 4;
	}

	for(size_t i = 0; i < data_size-2; i++)
	{
		if(lookup[i] == ',')
		{
			putchar('\t');
		}
		else
		{
			putchar(lookup[i]);
		}
	}
	putchar('\n');

	fclose(fp);
	free(lookup);
	
	return 0;
}
