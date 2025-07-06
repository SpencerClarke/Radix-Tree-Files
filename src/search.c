#include "radix_tree_file_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "main: enter the rdt file and a japanese word\n");
		return 4;
	}
	
	FILE *fp = fopen(argv[1], "rb");
	if(fp == NULL)
	{
		fprintf(stderr, "main: failed to open file '%s'\n", argv[1]);
		return 4;
	}

	if(argc > 2)
	{
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
		
		free(lookup);
	}
	else
	{

		char input_buffer[2048];

		while((fgets(input_buffer, sizeof(input_buffer), stdin) != NULL))
		{
			size_t data_size;
			char *lookup;

			if(strlen(input_buffer) == sizeof(input_buffer) - 1)
			{
				fprintf(stderr, "Not found\n");
				continue;
			}
			
			input_buffer[strlen(input_buffer) - 1] = '\0'; /* Get rid of newline */

			lookup = (char *)radix_tree_file_lookup(fp, (uint8_t *)input_buffer, strlen(input_buffer), &data_size);
			if(lookup == NULL)
			{
				fprintf(stderr, "Not found\n");
				free(lookup);
				continue;
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

			free(lookup);
		}
	}
	fclose(fp);
	
	return 0;
}
