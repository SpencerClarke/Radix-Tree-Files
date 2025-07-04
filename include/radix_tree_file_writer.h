#ifndef RADIX_TREE_FILE_WRITER_H
#define RADIX_TREE_FILE_WRITER_H

#include "radix_tree.h"
#include <stdint.h>

struct Radix_Tree_Value
{
	uint32_t value_size;
	uint8_t *value;

	uint32_t file_offset;
	struct Radix_Tree_Value *next;
};


/*	left offset
 *	right offset
 *	value offset
 *	key bit count
 *	key bits
 */
#define TREE_NODE_FILE_SIZE(node) (4 + 4 + 4 + 4 + ((node)->key_bit_count + 7)/8)

struct Radix_Tree_File_Writer
{
	struct Radix_Tree_Value *value_list_root;
	struct Radix_Tree_Node *radix_tree_root;

	FILE *fp;
};

struct Radix_Tree_File_Writer radix_tree_file_writer_init(char *filename);
struct Radix_Tree_Value *radix_tree_file_writer_value_init(struct Radix_Tree_File_Writer *writer, uint32_t value_size, uint8_t *value);
void radix_tree_file_writer_insert(struct Radix_Tree_File_Writer *writer, uint8_t *key, struct Radix_Tree_Value *value);
struct Radix_Tree_Value *radix_tree_file_writer_value_lookup(struct Radix_Tree_File_Writer *writer, uint8_t *key);
void radix_tree_file_writer_modify_value(struct Radix_Tree_File_Writer *writer, uint8_t *key, uint32_t new_value_size, uint8_t *new_value);
void radix_tree_file_writer_write(struct Radix_Tree_File_Writer *writer);
void radix_tree_file_writer_destroy(struct Radix_Tree_File_Writer *writer);

#endif
