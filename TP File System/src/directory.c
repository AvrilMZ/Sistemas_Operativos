#include <string.h>
#include "directory.h"

void
directory_init(directory_t *dir)
{
	dir->entry_count = 2;  // . y ..
	strncpy(dir->entries[0].name, ".", MAX_FILENAME_LEN);
	dir->entries[0].inode_number = 0;
	strncpy(dir->entries[1].name, "..", MAX_FILENAME_LEN);
	dir->entries[1].inode_number = 0;
}

int
directory_lookup(directory_t *dir, const char *name)
{
	if (!dir || !name) {
		return -1;
	}

	int return_value = -1;
	for (int i = 0; i < dir->entry_count; i++) {
		if (strcmp(dir->entries[i].name, name) == 0) {
			return_value = dir->entries[i].inode_number;
			break;
		}
	}
	return return_value;
}

int
directory_add_entry(directory_t *dir, const char *name, uint32_t inode_number)
{
	if (!dir || !name || strlen(name) == 0 ||
	    strlen(name) >= MAX_FILENAME_LEN) {
		return -1;
	}

	if (dir->entry_count >= MAX_BLOCKS) {
		return -1;
	}
	for (int i = 0; i < dir->entry_count; i++) {
		if (strcmp(dir->entries[i].name, name) == 0) {
			return -1;
		}
	}
	strncpy(dir->entries[dir->entry_count].name, name, MAX_FILENAME_LEN);
	dir->entries[dir->entry_count].name[MAX_FILENAME_LEN - 1] = '\0';
	dir->entries[dir->entry_count].inode_number = inode_number;
	dir->entry_count++;
	return 0;
}

int
directory_remove_entry(directory_t *dir, const char *name)
{
	if (!dir || !name || strlen(name) == 0) {
		return -1;
	}

	for (int i = 0; i < dir->entry_count; i++) {
		if (strcmp(dir->entries[i].name, name) == 0) {
			for (int j = i; j < dir->entry_count - 1; j++) {
				dir->entries[j] = dir->entries[j + 1];
			}
			dir->entry_count--;
			return 0;
		}
	}
	return -1;
}

int
directory_list_entries(directory_t *dir, char buffer[][MAX_FILENAME_LEN])
{
	if (!dir) {
		return 0;
	}
	for (int i = 0; i < dir->entry_count; i++) {
		strcpy(buffer[i], dir->entries[i].name);
	}
	return dir->entry_count;
}

directory_t *
get_directory_by_inode(filesystem_t *fs, inode_t *inode)
{
	if (!fs || inode->inode_type != INODE_DIR) {
		return NULL;
	}

	for (int i = 0; i < NUM_DIRECT_BLOCKS; i++) {
		int block_number = inode_get_direct_block(inode, i);
		if (block_number != -1) {
			return (directory_t *) &fs->data_block[block_number].data;
		}
	}

	return NULL;
}