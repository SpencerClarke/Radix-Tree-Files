#ifndef RADIX_TREE_H
#define RADIX_TREE_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct Radix_Tree_Node
{
	size_t key_bit_count;
	char *key_bits;

	void *data;
	
	struct Radix_Tree_Node *left;
	struct Radix_Tree_Node *right;
};

void radix_tree_insert(struct Radix_Tree_Node **root, char *key, size_t key_size, void *data);
void *radix_tree_lookup(struct Radix_Tree_Node *root, char *key, size_t key_size);
void radix_tree_destroy(struct Radix_Tree_Node *root, void (*destructor)(void *));

#endif
