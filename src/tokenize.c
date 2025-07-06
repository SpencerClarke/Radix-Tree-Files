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
	char *sentence = malloc(strlen(argv[2] + 1));
	strcpy(sentence, argv[2]);
	size_t data_size;
	

	size_t sentence_length = strlen(sentence);
	char *current_head = sentence + sentence_length;
	char **tokens = calloc(sentence_length, sizeof(char *));
	size_t current_token_index = 0;
	while(current_head != sentence)
	{
		char *earliest = NULL;
		while(current_head >= sentence)
		{
			char *lookup = (char *)radix_tree_file_lookup(fp, (uint8_t *)current_head, strlen(current_head), &data_size);
			if(lookup != NULL)
			{
				earliest = current_head;
				free(lookup);
			}
			current_head--;
		}
		if(earliest == NULL)
		{
			break;
		}
		else
		{
			tokens[current_token_index] = malloc(strlen(earliest) + 1);
			strcpy(tokens[current_token_index], earliest);
			current_token_index++;
			*earliest = '\0';
			current_head = earliest;
		}
	}
	
	for(int i = current_token_index - 1; i >= 0; i--)
	{
		puts(tokens[i]);
		free(tokens[i]);
	}
	free(tokens);
	
	free(sentence);
	fclose(fp);

	return 0;
}
