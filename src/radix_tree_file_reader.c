#include "radix_tree_file_reader.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define READ_BIT(bytes, i) (((bytes)[(i) / 8] & ((1 << (8-1)) >> ((i) % 8))) ? 1 : 0)
#define SET_BIT(bytes, i) ((bytes)[(i) / 8] |= ((1 << (8-1)) >>  ((i) % 8)))
#define UNSET_BIT(bytes,i) ((bytes)[(i) / 8] &= ~((1 << (8-1)) >> ((i) % 8)))
#define WRITE_BIT(bytes, i, bit) ((bit) ? SET_BIT((bytes), (i)) : UNSET_BIT((bytes), (i)))

#define KEY_BIT_BUFFER_SIZE 2048

struct Radix_Tree_File_Node
{
	uint32_t left_offset;
	uint32_t right_offset;
	uint32_t value_offset;
	uint32_t key_bit_count;

	uint8_t key_bits[KEY_BIT_BUFFER_SIZE];
};


static struct Radix_Tree_File_Node load(FILE *fp)
{
	struct Radix_Tree_File_Node out;

	fread(&(out.left_offset), 4, 1, fp);
	fread(&(out.right_offset), 4, 1, fp);
	fread(&(out.value_offset), 4, 1, fp);
	fread(&(out.key_bit_count), 4, 1, fp);

	size_t key_bit_size = (out.key_bit_count + 7)/8;

	fread(&(out.key_bits), key_bit_size, key_bit_size, fp);

	return out;
}
uint8_t *radix_tree_file_lookup(FILE *fp, uint8_t *key, size_t key_size, size_t *size)
{
	size_t current_key_bit;
	size_t current_node_bit;

	size_t key_bit_count = key_size * 8;
	uint32_t current_node_offset = 0;
	int first = 1;

	struct Radix_Tree_File_Node current_node;

	fseek(fp, current_node_offset, SEEK_SET);
	current_node = load(fp);

	for(current_key_bit = current_node_bit = 0; current_key_bit < key_bit_count; current_key_bit++)
	{
		if(current_node_offset == 0 && !first)
		{
			return NULL;
		}

		if(current_node_bit >= current_node.key_bit_count)
		{
			current_node_bit = 0;
			if(READ_BIT(key, current_key_bit))
			{
				current_node_offset = current_node.right_offset;

				fseek(fp, current_node_offset, SEEK_SET);
				current_node = load(fp);
				first = 0;
			}
			else
			{
				current_node_offset = current_node.left_offset;
				fseek(fp, current_node_offset, SEEK_SET);
				current_node = load(fp);
				first = 0;
			}
		}
		else if(READ_BIT(key, current_key_bit) != READ_BIT(current_node.key_bits, current_node_bit))
		{
			return NULL;	
		}
		else
		{
			current_node_bit++;
		}
	}

	if(current_node_offset == 0)
	{
		return NULL;
	}

	if(current_node_bit < current_node.key_bit_count)
	{
		return NULL;
	}

	if(current_node.value_offset == 0)
	{
		return NULL;
	}
	
	fseek(fp, current_node.value_offset, SEEK_SET);
	
	uint32_t data_size;

	fread(&data_size, 4, 1, fp);

	uint8_t *out = malloc(data_size);
	if(out == NULL)
	{
		fprintf(stderr, "radix_tree_file_lookup: malloc failed\n");
		exit(4);
	}
	
	fread(out, data_size, 1, fp);
	*size = data_size;

	return out;
}
