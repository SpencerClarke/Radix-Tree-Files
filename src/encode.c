#include "radix_tree_file_writer.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 16384
int main(void)
{
	char word_buffer[BUFFER_SIZE];
	char definition_buffer[BUFFER_SIZE];

	struct Radix_Tree_File_Writer writer = radix_tree_file_writer_init("jmdict.rdt");

	FILE *fp = fopen("parsed.txt", "r");
	int c;
	size_t buffer_index = 0;
	int state = 0;
	
	while((c = getc(fp)) != EOF)
	{
		if(buffer_index > BUFFER_SIZE)
		{
			fprintf(stderr, "main: something overflowed the buffer size\n");
			exit(4);
		}
		if(state == 0)
		{
			if(c == '\n')
			{			
				word_buffer[buffer_index] = '\0';
				buffer_index = 0;
				state = 1;
			}
			else
			{
				word_buffer[buffer_index++] = c;
			}
		}
		else if(state == 1)
		{
			if(c == '\n')
			{
				char *current_token;
				
				definition_buffer[buffer_index] = '\0';
				for(size_t i = 0; i < buffer_index; i++)
				{
					if(definition_buffer[i] == '\t')
					{
						definition_buffer[i] = ',';
					}
				}
				current_token = strtok(word_buffer, "\t");
				struct Radix_Tree_Value *value = NULL;
				while(current_token != NULL)
				{
					struct Radix_Tree_Value *lookup_value = radix_tree_file_writer_value_lookup(&writer, (uint8_t *)current_token, strlen(current_token));
					if(lookup_value == NULL)
					{
						value = radix_tree_file_writer_value_init(&writer, strlen(definition_buffer) + 1, (uint8_t *)definition_buffer);
						radix_tree_file_writer_insert(&writer, (uint8_t *)current_token, strlen(current_token), value);
					}
					else
					{
						size_t new_string_size = strlen((char *)(lookup_value->value)) + strlen(definition_buffer) + 1;
						char *new_string = malloc(new_string_size);

						if(new_string == NULL)
						{
							fprintf(stderr, "main: failed to malloc\n");
							exit(4);
						}

						strcpy(new_string, (char *)(lookup_value->value));
						new_string[strlen(new_string)-1] = '\n';
						strcat(new_string, definition_buffer);

						radix_tree_file_writer_modify_value(&writer, (uint8_t *)current_token, strlen(current_token), new_string_size, (uint8_t *)new_string);
						free(new_string);
					}

					current_token = strtok(NULL, "\t");
				}

				buffer_index = 0;
				state = 0;
			}
			else
			{
				definition_buffer[buffer_index++] = c;
			}
		}
	}

	fclose(fp);	
	
	radix_tree_file_writer_write(&writer);
	radix_tree_file_writer_destroy(&writer);

	return 0;
}
