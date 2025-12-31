#ifndef INTMAP_H
#define INTMAP_H

#include <stdint.h>
#include "config.h"

typedef struct intmap {
	uint8_t inodes[MAX_INODES];
	uint8_t blocks[MAX_BLOCKS];
} intmap_t;

/**
 * Initializes all inode and block bits to 0 (unused).
 *
 * All bits in both intmaps are set to 0.
 */
void intmap_init(intmap_t *bm);

// -------------------- INODES --------------------
/**
 * Returns 1 if the given inode index is currently marked as used, 0 otherwise.
 * Does not modify the state of the intmap.
 */
int is_inode_used(intmap_t *bm, uint32_t inode_index);

/**
 * Marks the given inode index as used.
 *
 * The corresponding bit in `bm->inode_intmap[inode_index]` is set to 1.
 * All other bits in the inode intmap remain unchanged.
 */
void set_inode_used(intmap_t *bm, uint32_t inode_index);

/**
 * Marks the given inode index as unused.
 *
 * The corresponding bit in `bm->inode_intmap[inode_index]` is set to 0.
 * All other bits in the inode intmap remain unchanged.
 */
void clear_inode_used(intmap_t *bm, uint32_t inode_index);

/**
 * Returns the index of the first free inode, or -1 if none is available.
 *
 * Does not modify the intmap.
 */
int find_free_inode(intmap_t *bm);

// -------------------- BLOCKS --------------------
/**
 * Returns 1 if the given block index is currently marked as used, 0 otherwise.
 *
 * Does not modify the state of the intmap.
 */
int is_block_used(intmap_t *bm, uint32_t block_index);

/**
 * Marks the given block index as used.
 *
 * The corresponding bit in `bm->block_intmap[block_index]` is set to 1.
 * All other bits in the block intmap remain unchanged.
 */
void set_block_used(intmap_t *bm, uint32_t block_index);

/**
 * Marks the given block index as unused.
 *
 * The corresponding bit in `bm->block_intmap[block_index]` is set to 0.
 * All other bits in the block intmap remain unchanged.
 */
void clear_block_used(intmap_t *bm, uint32_t block_index);

/**
 * Returns the index of the first free block, or -1 if none is available.
 *
 * Does not modify the intmap.
 */
int find_free_block(intmap_t *bm);

#endif /* INTMAP_H */