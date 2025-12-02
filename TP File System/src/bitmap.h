#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include "config.h"

typedef struct bitmap {
	uint8_t inodes[MAX_INODES];
	uint8_t blocks[MAX_BLOCKS];
} bitmap_t;

/**
 * Initializes all inode and block bits to 0 (unused).
 *
 * All bits in both bitmaps are set to 0.
 */
void bitmap_init(bitmap_t *bm);

// -------------------- INODES --------------------
/**
 * Returns 1 if the given inode index is currently marked as used, 0 otherwise.
 * Does not modify the state of the bitmap.
 */
int is_inode_used(bitmap_t *bm, uint32_t inode_index);

/**
 * Marks the given inode index as used.
 *
 * The corresponding bit in `bm->inode_bitmap[inode_index]` is set to 1.
 * All other bits in the inode bitmap remain unchanged.
 */
void set_inode_used(bitmap_t *bm, uint32_t inode_index);

/**
 * Marks the given inode index as unused.
 *
 * The corresponding bit in `bm->inode_bitmap[inode_index]` is set to 0.
 * All other bits in the inode bitmap remain unchanged.
 */
void clear_inode_used(bitmap_t *bm, uint32_t inode_index);

/**
 * Returns the index of the first free inode, or -1 if none is available.
 *
 * Does not modify the bitmap.
 */
int find_free_inode(bitmap_t *bm);

// -------------------- BLOCKS --------------------
/**
 * Returns 1 if the given block index is currently marked as used, 0 otherwise.
 *
 * Does not modify the state of the bitmap.
 */
int is_block_used(bitmap_t *bm, uint32_t block_index);

/**
 * Marks the given block index as used.
 *
 * The corresponding bit in `bm->block_bitmap[block_index]` is set to 1.
 * All other bits in the block bitmap remain unchanged.
 */
void set_block_used(bitmap_t *bm, uint32_t block_index);

/**
 * Marks the given block index as unused.
 *
 * The corresponding bit in `bm->block_bitmap[block_index]` is set to 0.
 * All other bits in the block bitmap remain unchanged.
 */
void clear_block_used(bitmap_t *bm, uint32_t block_index);

/**
 * Returns the index of the first free block, or -1 if none is available.
 *
 * Does not modify the bitmap.
 */
int find_free_block(bitmap_t *bm);

#endif /* BITMAP_H */