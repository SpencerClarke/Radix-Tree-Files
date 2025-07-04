#ifndef RADIX_TREE_FILE_READER_H
#define RADIX_TREE_FILE_READER_H

#include <stdint.h>
#include <stdio.h>

/*	left offset
 *	right offset
 *	value offset
 *	key bit count
 *	key bits
 */

uint8_t *radix_tree_file_lookup(FILE *fp, uint8_t *key, size_t *size);

#endif
