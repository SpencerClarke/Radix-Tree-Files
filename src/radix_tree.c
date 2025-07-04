#include "radix_tree.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define READ_BIT(bytes, i) (((bytes)[(i) / CHAR_BIT] & ((1 << (CHAR_BIT-1)) >> ((i) % CHAR_BIT))) ? 1 : 0)
#define SET_BIT(bytes, i) ((bytes)[(i) / CHAR_BIT] |= ((1 << (CHAR_BIT-1)) >>  ((i) % CHAR_BIT)))
#define UNSET_BIT(bytes,i) ((bytes)[(i) / CHAR_BIT] &= ~((1 << (CHAR_BIT-1)) >> ((i) % CHAR_BIT)))
#define WRITE_BIT(bytes, i, bit) ((bit) ? SET_BIT((bytes), (i)) : UNSET_BIT((bytes), (i)))

static struct Radix_Tree_Node *radix_tree_node_init(char *data, char *key, size_t bit_start, size_t bit_length)
{
	struct Radix_Tree_Node *out;
	size_t current_bit;

	out = calloc(1, sizeof(struct Radix_Tree_Node));
	if(out == NULL)
	{
		fprintf(stderr, "radix_tree_node_init: failed to malloc\n");
		exit(4);
	}

	out->data = data;

	out->left = out->right = NULL;
	
	if(bit_length == 0)
	{
		out->key_bits = NULL;
		out->key_bit_count = 0;
		return out;
	}
	out->key_bit_count = bit_length;

	out->key_bits = calloc(1, (bit_length + CHAR_BIT - 1) / CHAR_BIT);
	if(out->key_bits == NULL)
	{
		fprintf(stderr, "radix_tree_node_init: failed to malloc\n");
		exit(4);
	}

	for(current_bit = 0; current_bit < bit_length; current_bit++)
	{
		WRITE_BIT(out->key_bits, current_bit, READ_BIT(key, bit_start + current_bit));
	}

	return out;
}

void radix_tree_insert(struct Radix_Tree_Node **root, char *key, size_t key_size, void *data)
{
	size_t current_key_bit;
	size_t current_node_bit;
	size_t key_bit_count = key_size * CHAR_BIT;

	struct Radix_Tree_Node **current_node;

	current_node = root;
	/* Traverse the tree */
	/* End up with current_node as a pointer to pointer to the node to insert the value into */
	/* Or a pointer to a NULL pointer to create a new node with that value */
	/* And end up with current_key_bit as the first bit to write into the key segment for that node */
	for(current_key_bit = current_node_bit = 0; current_key_bit < key_bit_count; current_key_bit++)
	{
		if(*current_node == NULL)
		{
			break;
		}
		else if(current_node_bit >= (*current_node)->key_bit_count)
		{
			current_node_bit = 0;
			if(READ_BIT(key, current_key_bit))
			{
				current_node = &((*current_node)->right);
			}
			else
			{
				current_node = &((*current_node)->left);
			}
		}
		else if(READ_BIT(key, current_key_bit) != READ_BIT((*current_node)->key_bits, current_node_bit))
		{
			/* Split on the mismatching bit */
			struct Radix_Tree_Node *split = radix_tree_node_init(
				NULL,
				(*current_node)->key_bits,
				0,
				current_node_bit
			);
			
			/* Update the current node's key */
			size_t new_bit_length = (*current_node)->key_bit_count - current_node_bit - 1;
			if(new_bit_length == 0)
			{
				free((*current_node)->key_bits);
				(*current_node)->key_bits = NULL;
			}
			else
			{
				size_t current_new_bit;
				for(current_new_bit = 0; current_new_bit < new_bit_length; current_new_bit++)
				{
					WRITE_BIT
					(
						(*current_node)->key_bits, 
						current_new_bit,
						READ_BIT
						(
							(*current_node)->key_bits,
							current_node_bit + current_new_bit + 1
						)
					);
				}
			}
			(*current_node)->key_bit_count = new_bit_length;

			if(READ_BIT(key, current_key_bit))
			{
				struct Radix_Tree_Node *temp = *current_node;
				*current_node = split;
				split->left = temp;
				current_node = &(split->right);
			}
			else
			{
				struct Radix_Tree_Node *temp = *current_node;
				*current_node = split;
				split->right = temp;
				current_node = &(split->left);
			}
			
		}
		else
		{
			current_node_bit++;
		}
	}

	if((*current_node) == NULL)
	{
		(*current_node) = radix_tree_node_init(
			data,
			key,
			current_key_bit,
			key_bit_count - current_key_bit
		);
	}

	else if(current_node_bit < (*current_node)->key_bit_count)
	{
		/* We stopped half-way inside a node */
		
		/* Create a new node that ends at the half way point */
		struct Radix_Tree_Node *new = radix_tree_node_init(
			data,
			(*current_node)->key_bits,
			0,
			current_node_bit
		);
		
		if(READ_BIT((*current_node)->key_bits, current_node_bit))
		{
			new->right = *current_node;
		}
		else
		{
			new->left = *current_node;
		}

		/* Make the old node begin after the half way point */
		size_t current_new_bit;
		size_t new_bit_length = (*current_node)->key_bit_count - current_node_bit - 1;
		if(new_bit_length == 0)
		{
			free((*current_node)->key_bits);
			(*current_node)->key_bits = NULL;
		}
		else
		{
			for(current_new_bit = 0; current_new_bit < new_bit_length; current_new_bit++)
			{
				WRITE_BIT
				(
					(*current_node)->key_bits, 
					current_new_bit,
					READ_BIT
					(
						(*current_node)->key_bits,
						current_node_bit + current_new_bit + 1
					)
				);
			}
		}

		(*current_node)->key_bit_count = new_bit_length;

		*current_node = new;
	}
	else if((*current_node)->data == NULL)
	{
		(*current_node)->data = data;
	}
	else
	{
		fprintf(stderr, "radix_tree_insert: double insertion on key %s\n", key);
		exit(4);
	}
}
void *radix_tree_lookup(struct Radix_Tree_Node *root, char *key, size_t key_size)
{
	size_t current_key_bit;
	size_t current_node_bit;

	size_t key_bit_count = key_size * CHAR_BIT;
	struct Radix_Tree_Node *current_node = root;

	for(current_key_bit = current_node_bit = 0; current_key_bit < key_bit_count; current_key_bit++)
	{
		if(current_node == NULL)
		{
			return NULL;
		}
		else if(current_node_bit >= current_node->key_bit_count)
		{
			current_node_bit = 0;
			if(READ_BIT(key, current_key_bit))
			{
				current_node = current_node->right;
			}
			else
			{
				current_node = current_node->left;
			}
		}
		else if(READ_BIT(key, current_key_bit) != READ_BIT(current_node->key_bits, current_node_bit))
		{
			return NULL;	
		}
		else
		{
			current_node_bit++;
		}
	}

	if(current_node == NULL)
	{
		return NULL;
	}

	if(current_node_bit < current_node->key_bit_count)
	{
		return NULL;
	}
	
	return current_node->data;
}
void radix_tree_destroy(struct Radix_Tree_Node *root, void (*destructor)(void *))
{
	if(root == NULL)
	{
		return;
	}	

	if(destructor != NULL && root->data != NULL)
	{
		(*destructor)(root->data);
	}

	free(root->key_bits);
	
	radix_tree_destroy(root->left, destructor);
	radix_tree_destroy(root->right, destructor);

	free(root);
}

