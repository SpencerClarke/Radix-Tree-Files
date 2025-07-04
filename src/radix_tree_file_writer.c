#ifndef RADIX_TREE_FILE_H
#define RADIX_TREE_FILE_H

#include "radix_tree_file_writer.h"
#include "radix_tree.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Radix_Tree_File_Writer radix_tree_file_writer_init(char *filename)
{
	struct Radix_Tree_File_Writer out;
	
	out.fp = fopen(filename, "wb");
	if(out.fp == NULL)
	{
		fprintf(stderr, "radix_tree_file_writer_init: failed to open file '%s'\n", filename);
		exit(3);
	}

	out.radix_tree_root = NULL;
	out.value_list_root = NULL;

	return out;
}

struct Radix_Tree_Value *radix_tree_file_writer_value_lookup(struct Radix_Tree_File_Writer *writer, uint8_t *key)
{
	return radix_tree_lookup(writer->radix_tree_root, (char *)key);
}
struct Radix_Tree_Value *radix_tree_file_writer_value_init(struct Radix_Tree_File_Writer *writer, uint32_t value_size, uint8_t *value)
{
	struct Radix_Tree_Value *out;

	out = malloc(sizeof(struct Radix_Tree_Value));
	if(out == NULL)
	{
		fprintf(stderr, "radix_tree_value_init: failed to malloc\n");
		exit(4);
	}

	out->file_offset = 0; /* Temp */

	out->value = malloc(value_size);
	if(out->value == NULL)
	{
		fprintf(stderr, "radix_tree_value_init: failed to malloc\n");
		exit(4);
	}
	
	memcpy(out->value, value, value_size);
	out->value_size = value_size;

	out->next = writer->value_list_root;
	writer->value_list_root = out;

	return out;
}
void radix_tree_file_writer_insert(struct Radix_Tree_File_Writer *writer, uint8_t *key, struct Radix_Tree_Value *value)
{
	radix_tree_insert(&(writer->radix_tree_root), (char *)key, value);
}
void radix_tree_file_writer_modify_value(struct Radix_Tree_File_Writer *writer, uint8_t *key, uint32_t new_value_size, uint8_t *new_value)
{
	struct Radix_Tree_Value *value;
	if((value = radix_tree_lookup(writer->radix_tree_root, (char *)key)) == NULL)
	{
		fprintf(stderr, "radix_tree_filer_writer_modify_value: tried to modify value on non-existent key\n");
		exit(4);
	}
	
	value->value = realloc(value->value, new_value_size);
	if(value->value == NULL)
	{
		fprintf(stderr, "radix_tree_file_writer_modify_value: failed to realloc\n");
		exit(4);
	}
	memcpy(value->value, new_value, new_value_size);	
}

static size_t compute_radix_tree_file_size(struct Radix_Tree_Node *root)
{
	if(root == NULL)
	{
		return 0;
	}
	
	return TREE_NODE_FILE_SIZE(root) + compute_radix_tree_file_size(root->left) + compute_radix_tree_file_size(root->right);
}

/*	left offset
 *	right offset
 *	value offset
 *	key bit count
 *	key bits
 */
static uint32_t current_file_offset = 0;

uint32_t write_radix_tree_node(uint32_t left_offset, uint32_t right_offset, struct Radix_Tree_Node *node, FILE *fp)
{
	fwrite(&left_offset, 4, 1, fp);
	fwrite(&right_offset, 4, 1, fp);
	
	struct Radix_Tree_Value *value = node->data;
	if(value == NULL)
	{
		uint32_t null_ptr = 0;
		fwrite(&null_ptr, 4, 1, fp);
	}
	else
	{
		fwrite(&(value->file_offset), 4, 1, fp);
	}

	uint32_t key_bit_count = node->key_bit_count;
	fwrite(&key_bit_count, 4, 1, fp);
	
	if(node->key_bits != NULL)
	{
		fwrite(node->key_bits, (key_bit_count + 7) / 8, 1, fp);
	}
	return TREE_NODE_FILE_SIZE(node);
}
static uint32_t write_radix_tree(struct Radix_Tree_Node *root, FILE *fp)
{
	uint32_t out = 0;
	if(root == NULL)
	{
		return out;
	}

	uint32_t left_offset = write_radix_tree(root->left, fp);
	uint32_t right_offset = write_radix_tree(root->right, fp);

	out = current_file_offset;

	current_file_offset += write_radix_tree_node(left_offset, right_offset, root, fp);

	return out;
}

void radix_tree_file_writer_write(struct Radix_Tree_File_Writer *writer)
{
	if(writer->radix_tree_root == NULL)
	{
		return;
	}

	uint32_t current_value_offset = compute_radix_tree_file_size(writer->radix_tree_root);
	fseek(writer->fp, current_value_offset, SEEK_SET);

	struct Radix_Tree_Value *current = writer->value_list_root;
	while(current != NULL)
	{
		current->file_offset = current_value_offset;
		fwrite(&(current->value_size), 4, 1, writer->fp);
		fwrite(current->value, current->value_size, 1, writer->fp);

		current_value_offset += 4 + current->value_size;
		current = current->next;
	}

	current_file_offset = TREE_NODE_FILE_SIZE(writer->radix_tree_root);
	fseek(writer->fp, current_file_offset, SEEK_SET);

	uint32_t left_offset = write_radix_tree(writer->radix_tree_root->left, writer->fp);
	uint32_t right_offset = write_radix_tree(writer->radix_tree_root->right, writer->fp);

	fseek(writer->fp, 0, SEEK_SET);

	write_radix_tree_node(left_offset, right_offset, writer->radix_tree_root, writer->fp);
}

void radix_tree_file_writer_destroy(struct Radix_Tree_File_Writer *writer)
{
	struct Radix_Tree_Value *current = writer->value_list_root;
	struct Radix_Tree_Value *prev;
	while(current != NULL)
	{
		prev = current;
		current = current->next;
		free(prev->value);
		free(prev);
	}

	radix_tree_destroy(writer->radix_tree_root, NULL);
}

#endif
