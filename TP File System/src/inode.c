#include <unistd.h>

#include "filesystem.h"
#include "inode.h"

void
inode_init(inode_t *inode)
{
	if (!inode) {
		return;
	}
	inode->file_size_bytes = 0;
	inode->owner_user_id = getuid();
	inode->owner_group_id = getgid();
	inode->permission_bits = PERM_READ;  // Modo lectura por default
	time_t current_time;
	inode->last_access_time = time(&current_time);
	inode->last_modification_time = time(&current_time);
	inode->last_metadata_change_time = time(&current_time);
	inode->hard_links_count = 1;  // Un nuevo archivo ya tiene una referencia
	for (int i = 0; i < NUM_DIRECT_BLOCKS; i++) {
		inode->direct_block_ptrs[i] = -1;
	}
	inode->inode_type = INODE_FILE;
}

int
allocate_inode(filesystem_t *fs)
{
	if (!fs) {
		return -1;
	}
	for (int i = 0; i < MAX_INODES; i++) {
		if (!is_inode_used(&fs->intmap, i)) {
			set_inode_used(&fs->intmap, i);
			inode_init(&fs->inodes[i]);
			return i;
		}
	}
	return -1;
}

void
inode_update_atime(inode_t *inode)
{
	if (!inode) {
		return;
	}
	time_t current_time;
	inode->last_access_time = time(&current_time);
}

void
inode_update_mtime(inode_t *inode)
{
	if (!inode) {
		return;
	}
	time_t current_time;
	inode->last_modification_time = time(&current_time);
}

void
inode_update_ctime(inode_t *inode)
{
	if (!inode) {
		return;
	}
	time_t current_time;
	inode->last_metadata_change_time = time(&current_time);
}

void
inode_increment_links(inode_t *inode)
{
	if (!inode) {
		return;
	}
	inode->hard_links_count++;
}

void
inode_decrement_links(inode_t *inode)
{
	if (!inode) {
		return;
	}
	inode->hard_links_count--;
}

int
inode_add_direct_block(filesystem_t *fs, inode_t *inode, uint32_t block_number)
{
	if (!fs || !inode || block_number >= MAX_BLOCKS) {
		return -1;
	}
	for (int i = 0; i < NUM_DIRECT_BLOCKS; i++) {
		if (inode->direct_block_ptrs[i] == -1) {
			inode->direct_block_ptrs[i] = block_number;
			set_block_used(&fs->intmap, block_number);
			return 0;
		}
	}
	return -1;  // No hay espacio para más bloques directos
}

int
inode_get_direct_block(inode_t *inode, int index)
{
	if (!inode || index < 0 || index >= NUM_DIRECT_BLOCKS) {
		return -1;
	}
	if (inode->direct_block_ptrs[index] == -1) {
		return -1;
	}
	return inode->direct_block_ptrs[index];
}

void
inode_free(filesystem_t *fs, int inode_index)
{
	if (!fs || inode_index < 0 || inode_index >= MAX_INODES) {
		return;
	}
	inode_t *inode = &fs->inodes[inode_index];
	for (int i = 0; i < NUM_DIRECT_BLOCKS; i++) {
		if (inode->direct_block_ptrs[i] != -1) {
			clear_block_used(&fs->intmap, inode->direct_block_ptrs[i]);
			inode->direct_block_ptrs[i] = -1;
		}
	}
	clear_inode_used(&fs->intmap, inode_index);
	inode_init(inode);
}