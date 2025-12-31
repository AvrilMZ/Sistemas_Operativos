#include "intmap.h"

void
intmap_init(intmap_t *bm)
{
	if (!bm) {
		return;
	}
	for (int i = 0; i < MAX_INODES; i++) {
		bm->inodes[i] = 0;
	}
	for (int i = 0; i < MAX_BLOCKS; i++) {
		bm->blocks[i] = 0;
	}
}

int
is_inode_used(intmap_t *bm, uint32_t inode_index)
{
	if (!bm || inode_index >= MAX_INODES) {
		return -1;
	}
	return bm->inodes[inode_index] ? 1 : 0;
}

void
set_inode_used(intmap_t *bm, uint32_t inode_index)
{
	if (!bm || inode_index >= MAX_INODES) {
		return;
	}
	bm->inodes[inode_index] = 1;
}

void
clear_inode_used(intmap_t *bm, uint32_t inode_index)
{
	if (!bm || inode_index >= MAX_INODES) {
		return;
	}
	bm->inodes[inode_index] = 0;
}

int
find_free_inode(intmap_t *bm)
{
	if (!bm) {
		return -1;
	}
	int index = -1;
	int found = 0;
	for (int i = 1; i < MAX_INODES && !found;
	     i++) {  // block 0 siempre de root
		if (!is_inode_used(bm, i)) {
			index = i;
			found = 1;
		}
	}
	return index;
}

int
is_block_used(intmap_t *bm, uint32_t block_index)
{
	if (!bm || block_index >= MAX_BLOCKS) {
		return -1;
	}
	return bm->blocks[block_index] ? 1 : 0;
}

void
set_block_used(intmap_t *bm, uint32_t block_index)
{
	if (!bm || block_index >= MAX_BLOCKS) {
		return;
	}
	bm->blocks[block_index] = 1;
}

void
clear_block_used(intmap_t *bm, uint32_t block_index)
{
	if (!bm || block_index >= MAX_BLOCKS) {
		return;
	}
	bm->blocks[block_index] = 0;
}

int
find_free_block(intmap_t *bm)
{
	if (!bm) {
		return -1;
	}
	for (int i = 0; i < MAX_BLOCKS; i++) {
		if (!is_block_used(bm, i)) {
			return i;
		}
	}
	return -1;
}