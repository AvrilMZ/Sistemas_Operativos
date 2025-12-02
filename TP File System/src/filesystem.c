#include <unistd.h>
#include <string.h>

#include "filesystem.h"
#include "directory.h"
#include <stdio.h>


void
fs_init(filesystem_t *fs)
{
	if (!fs) {
		return;
	}

	// superblock
	fs->superblock.total_inodes = MAX_INODES;
	fs->superblock.total_blocks = MAX_BLOCKS;
	fs->superblock.root_inode_number = 0;
	fs->superblock.free_inodes_count = MAX_INODES - 1;  // 1 para root
	fs->superblock.free_blocks_count = MAX_BLOCKS - 1;  // 1 para root
	fs->superblock.block_size_bytes = BLOCK_SIZE;
	fs->superblock.max_file_size_bytes = NUM_DIRECT_BLOCKS * BLOCK_SIZE;
	fs->superblock.max_filename_length = MAX_FILENAME_LEN;
	fs->superblock.max_path_length = MAX_PATH_LEN;

	// bitmaps
	bitmap_init(&fs->bitmap);

	// inodes
	for (size_t i = 0; i < MAX_INODES; i++) {
		inode_init(&fs->inodes[i]);
	}

	// init root
	set_block_used(&fs->bitmap, 0);
	set_inode_used(&fs->bitmap, 0);

	inode_t *root = &fs->inodes[0];
	root->inode_type = INODE_DIR;
	root->permission_bits = PERM_RWX;
	root->owner_user_id = 0;
	root->owner_group_id = 0;
	for (int i = 0; i < NUM_DIRECT_BLOCKS; i++) {
		root->direct_block_ptrs[i] = -1;
	}
	root->direct_block_ptrs[0] = 0;
	root->file_size_bytes = sizeof(directory_t);
	time_t current_time = time(NULL);
	root->last_modification_time = current_time;
	root->last_metadata_change_time = current_time;
	root->last_access_time = current_time;

	root->direct_block_ptrs[0] = 0;
	directory_t *root_dir = (directory_t *) &fs->data_block[0].data;
	directory_init(root_dir);
}

int
fs_create_file(filesystem_t *fs, const char *path, mode_t mode)
{
	if (!path || !fs || path[0] == '\0') {
		return -1;
	}

	if (strlen(path) >= MAX_PATH_LEN) {
		return -ENAMETOOLONG;
	}

	inode_t *parent_dir_inode;
	char filename[MAX_FILENAME_LEN];
	if (fs_resolve_parent(fs, path, &parent_dir_inode, filename) < 0) {
		return -1;
	}

	directory_t *parent_dir = get_directory_by_inode(fs, parent_dir_inode);
	if (!parent_dir) {
		return -1;
	}
	if (directory_lookup(parent_dir, filename) >= 0) {
		return -EEXIST;
	}

	int inode_indx = allocate_inode(fs);
	if (inode_indx < 0) {
		return -ENOSPC;
	}

	fs->superblock.free_inodes_count--;

	inode_t *inode = &fs->inodes[inode_indx];
	inode->permission_bits = mode;
	inode->owner_user_id = getuid();
	inode->owner_group_id = getgid();
	inode->file_size_bytes = 0;
	inode->inode_type = INODE_FILE;
	for (int i = 0; i < NUM_DIRECT_BLOCKS; i++) {
		inode->direct_block_ptrs[i] = -1;
	}

	int block_free = find_free_block(&fs->bitmap);
	if (block_free < 0 || inode_add_direct_block(fs, inode, block_free) == -1) {
		inode_free(fs, inode_indx);
		fs->superblock.free_inodes_count++;
		return -ENOSPC;
	}

	fs->superblock.free_blocks_count--;
	int dir = directory_add_entry(parent_dir, filename, inode_indx);
	if (dir != 0) {
		inode_free(fs, inode_indx);
		fs->superblock.free_inodes_count++;
		fs->superblock.free_blocks_count++;
		return -1;
	}

	return 0;
}

int
fs_read_file(filesystem_t *fs, const char *path, char *buffer, size_t size, off_t offset)
{
	if (!fs || !path || !buffer) {
		return -1;
	}

	inode_t *parent_dir_inode;
	char filename[MAX_FILENAME_LEN];
	if (fs_resolve_parent(fs, path, &parent_dir_inode, filename) < 0) {
		return -1;
	}

	directory_t *parent_dir = get_directory_by_inode(fs, parent_dir_inode);
	if (!parent_dir) {
		return -1;
	}

	int inode_idx = directory_lookup(parent_dir, filename);
	if (inode_idx < 0) {
		return -1;
	}

	inode_t *inode = &fs->inodes[inode_idx];
	if (offset >= inode->file_size_bytes) {
		return 0;
	}

	size_t bytes_left = inode->file_size_bytes - offset;
	size_t bytes_to_read = (size < bytes_left) ? size : bytes_left;
	size_t bytes_read = 0;
	size_t block_size = fs->superblock.block_size_bytes;
	size_t block_idx = offset / block_size;
	size_t block_offset = offset % block_size;

	while (bytes_read < bytes_to_read) {
		if (block_idx >= NUM_DIRECT_BLOCKS) {
			break;
		}

		int block_num = inode_get_direct_block(inode, block_idx);
		if (block_num < 0) {
			break;
		}

		uint8_t *data = fs->data_block[block_num].data;
		size_t block_bytes_available = block_size - block_offset;
		size_t rest = bytes_to_read - bytes_read;
		size_t copy_size = (rest < block_bytes_available)
		                           ? rest
		                           : block_bytes_available;
		memcpy(buffer + bytes_read, data + block_offset, copy_size);
		bytes_read += copy_size;
		block_offset = 0;
		block_idx++;
	}

	inode_update_atime(inode);
	return (int) bytes_read;
}

int
fs_write_file(filesystem_t *fs,
              const char *path,
              const char *buffer,
              size_t size,
              off_t offset,
              int append)
{
	if (!fs || !path || !buffer || size == 0) {
		return -1;
	}

	inode_t *parent_dir_inode;
	char filename[MAX_FILENAME_LEN];
	if (fs_resolve_parent(fs, path, &parent_dir_inode, filename) < 0) {
		return -1;
	}

	directory_t *parent_dir = get_directory_by_inode(fs, parent_dir_inode);
	if (!parent_dir) {
		return -1;
	}

	int inode_idx = directory_lookup(parent_dir, filename);
	if (inode_idx < 0) {
		return -1;
	}

	inode_t *inode = &fs->inodes[inode_idx];
	if ((inode->permission_bits & PERM_WRITE) == 0) {
		return -1;
	}

	if (append) {
		offset = inode->file_size_bytes;
	}

	size_t bytes_written = 0;
	size_t current_pos = offset;
	size_t block_size = fs->superblock.block_size_bytes;

	while (bytes_written < size) {
		size_t block_index = current_pos / block_size;
		size_t block_offset = current_pos % block_size;
		if (block_index >= NUM_DIRECT_BLOCKS) {
			break;
		}

		int block_num = inode_get_direct_block(inode, block_index);
		if (block_num < 0) {
			int new_block_idx = find_free_block(&fs->bitmap);
			if (new_block_idx == -1) {
				break;
			}

			if (inode_add_direct_block(fs, inode, new_block_idx) ==
			    -1) {
				break;
			}

			fs->superblock.free_blocks_count--;
			block_num = new_block_idx;
		}

		data_block_t *current_block = &fs->data_block[block_num];
		size_t aux = block_size - block_offset;
		size_t bytes_to_copy =
		        (size - bytes_written > aux) ? aux : size - bytes_written;
		memcpy(current_block->data + block_offset,
		       buffer + bytes_written,
		       bytes_to_copy);
		bytes_written += bytes_to_copy;
		current_pos += bytes_to_copy;
	}

	if (current_pos > inode->file_size_bytes) {
		inode->file_size_bytes = current_pos;
	}

	inode_update_mtime(inode);
	inode_update_ctime(inode);
	inode_update_atime(inode);

	return bytes_written;
}

int
fs_delete_file(filesystem_t *fs, const char *path)
{
	if (!fs || !path) {
		return -1;
	}

	inode_t *parent_dir_inode;
	char filename[MAX_FILENAME_LEN];
	if (fs_resolve_parent(fs, path, &parent_dir_inode, filename) < 0) {
		return -1;
	}

	directory_t *parent_dir = get_directory_by_inode(fs, parent_dir_inode);
	if (!parent_dir) {
		return -1;
	}

	int inode_idx = directory_lookup(parent_dir, filename);
	if (inode_idx < 0) {
		return -ENOENT;
	}

	inode_t *inode = &fs->inodes[inode_idx];
	if (inode->inode_type != INODE_FILE && inode->inode_type != INODE_SYMLINK) {
		return -EISDIR;
	}

	if (directory_remove_entry(parent_dir, filename) != 0) {
		return -EIO;
	}

	inode_decrement_links(inode);
	if (inode->hard_links_count > 0) {
		return 0;
	}

	for (int i = 0; i < NUM_DIRECT_BLOCKS; i++) {
		int block_idx = inode_get_direct_block(inode, i);
		if (block_idx != -1) {
			clear_block_used(&fs->bitmap, block_idx);
			memset(fs->data_block[block_idx].data, 0, BLOCK_SIZE);
			fs->superblock.free_blocks_count++;
			inode->direct_block_ptrs[i] = -1;
		}
	}

	clear_inode_used(&fs->bitmap, inode_idx);
	inode_init(inode);
	fs->superblock.free_inodes_count++;

	inode_update_mtime(&fs->inodes[fs->superblock.root_inode_number]);
	inode_update_ctime(&fs->inodes[fs->superblock.root_inode_number]);

	return 0;
}

int
fs_create_directory(filesystem_t *fs, const char *path, mode_t mode, char *name)
{
	if (!fs || !path || path[0] == '\0' || !name) {
		return -1;
	}

	if (strlen(path) >= MAX_PATH_LEN) {
		return -ENAMETOOLONG;
	}

	inode_t *parent_dir_inode;
	char filename[MAX_FILENAME_LEN];
	if (fs_resolve_parent(fs, path, &parent_dir_inode, filename) < 0) {
		return -1;
	}

	directory_t *parent_dir = get_directory_by_inode(fs, parent_dir_inode);
	if (!parent_dir) {
		return -1;
	}

	if (directory_lookup(parent_dir, filename) >= 0) {
		return -EEXIST;
	}

	int new_inode_idx = allocate_inode(fs);
	if (new_inode_idx < 0 || new_inode_idx >= MAX_INODES) {
		return -ENOSPC;
	}

	fs->superblock.free_inodes_count--;

	inode_t *new_inode = &fs->inodes[new_inode_idx];
	inode_init(new_inode);
	new_inode->inode_type = INODE_DIR;
	new_inode->owner_user_id = getuid();
	new_inode->owner_group_id = getgid();
	new_inode->permission_bits = mode;
	new_inode->file_size_bytes = sizeof(directory_t);

	int new_block_idx = find_free_block(&fs->bitmap);
	if (new_block_idx < 0) {
		inode_free(fs, new_inode_idx);
		fs->superblock.free_inodes_count++;
		return -ENOSPC;
	}

	if (inode_add_direct_block(fs, new_inode, new_block_idx) == -1) {
		inode_free(fs, new_inode_idx);
		clear_block_used(&fs->bitmap, new_block_idx);
		fs->superblock.free_inodes_count++;
		return -ENOSPC;
	}

	fs->superblock.free_blocks_count--;

	// Inicializa el nuevo directorio ('.' y '..')
	directory_t *new_dir = (directory_t *) &fs->data_block[new_block_idx].data;
	directory_init(new_dir);
	directory_add_entry(new_dir, ".", new_inode_idx);
	int parent_inode_idx = parent_dir_inode - fs->inodes;
	directory_add_entry(new_dir, "..", parent_inode_idx);

	// Agrega la entrada en el directorio padre
	if (directory_add_entry(parent_dir, name, new_inode_idx) == -1) {
		inode_free(fs, new_inode_idx);
		clear_block_used(&fs->bitmap, new_block_idx);
		fs->superblock.free_inodes_count++;
		fs->superblock.free_blocks_count++;
		return -EIO;
	}

	return 0;
}

int
fs_list_directory(filesystem_t *fs,
                  const char *path,
                  char buffer[][MAX_FILENAME_LEN])
{
	if (!fs || !path || path[0] == '\0') {
		return -1;
	}

	if (strlen(path) >= MAX_PATH_LEN) {
		return -ENAMETOOLONG;
	}

	int inode_number = fs_resolve_path(fs, path);
	if (inode_number == -1) {
		return -ENOENT;
	}

	inode_t *inode = &fs->inodes[inode_number];
	if (inode->inode_type != INODE_DIR) {
		return -ENOTDIR;
	}

	directory_t *dir = get_directory_by_inode(fs, inode);
	if (!dir) {
		return -EIO;
	}

	int count = directory_list_entries(dir, buffer);
	inode_update_atime(inode);
	return count;
}

int
fs_remove_directory(filesystem_t *fs, const char *path)
{
	if (!fs || !path || path[0] == '\0') {
		return -1;
	}

	if (strlen(path) >= MAX_PATH_LEN) {
		return -ENAMETOOLONG;
	}

	int inode_number = fs_resolve_path(fs, path);
	if (inode_number == -1) {
		return -ENOENT;
	}

	inode_t *inode = &fs->inodes[inode_number];
	if (inode->inode_type != INODE_DIR) {
		return -ENOTDIR;
	}

	directory_t *dir = get_directory_by_inode(fs, inode);
	if (!dir) {
		return -EIO;
	}
	if (dir->entry_count > 2) {
		return -ENOTEMPTY;
	}

	inode_t *parent_dir_inode;
	char target_name[MAX_FILENAME_LEN];
	if (fs_resolve_parent(fs, path, &parent_dir_inode, target_name) < 0) {
		return -EIO;
	}

	directory_t *parent_dir = get_directory_by_inode(fs, parent_dir_inode);
	if (!parent_dir) {
		return -EIO;
	}

	if (directory_remove_entry(parent_dir, target_name) < 0) {
		return -EIO;
	}

	inode_free(fs, inode_number);
	inode_update_ctime(inode);
	inode_update_ctime(parent_dir_inode);
	inode_update_mtime(parent_dir_inode);
	fs->superblock.free_inodes_count++;

	return 0;
}

int
fs_statistics(filesystem_t *fs, const char *path, struct stat *st)
{
	if (!fs || !path || !st) {
		return -1;
	}

	int inode_number = fs_resolve_path(fs, path);
	if (inode_number == -1) {
		return -ENOENT;
	}

	inode_t *inode = &fs->inodes[inode_number];
	memset(st, 0, sizeof(struct stat));
	st->st_uid = inode->owner_user_id;
	st->st_gid = inode->owner_group_id;
	st->st_atime = inode->last_access_time;
	st->st_mtime = inode->last_modification_time;
	st->st_ctime = inode->last_metadata_change_time;
	st->st_size = inode->file_size_bytes;

	if (inode->inode_type == INODE_DIR) {
		st->st_mode = FS_MODE_DIR | inode->permission_bits;
	} else if (inode->inode_type == INODE_FILE) {
		st->st_mode = FS_MODE_FILE | inode->permission_bits;
	} else if (inode->inode_type == INODE_SYMLINK) {
		st->st_mode = FS_MODE_SYMLINK | inode->permission_bits;
	}

	inode_update_atime(inode);
	return 0;
}

int
fs_resolve_path(filesystem_t *fs, const char *path)
{
	printf("[DEBUG] RESOLVE PATH - input '%s'\n", path);
	if (!fs || !path || path[0] == '\0') {
		return -1;
	}

	if (strlen(path) >= MAX_PATH_LEN) {
		return -ENAMETOOLONG;
	}

	// Chequeamos si el path es el root
	if (strcmp(path, NAME_ROOT) == 0) {
		return fs->superblock.root_inode_number;
	}
	char aux_path[MAX_PATH_LEN];
	strncpy(aux_path, path, MAX_PATH_LEN);
	aux_path[MAX_PATH_LEN - 1] = '\0';

	uint32_t current_inode_number = fs->superblock.root_inode_number;
	inode_t *current_inode = &fs->inodes[current_inode_number];
	inode_update_atime(current_inode);

	int depth = 0;
	char *token = strtok(aux_path, "/");
	while (token) {
		if (depth > MAX_DIR_DEPTH) {
			return -ENAMETOOLONG;
		}
		depth++;

		directory_t *current_dir =
		        get_directory_by_inode(fs, current_inode);
		if (!current_dir) {
			return -1;
		}

		current_inode_number = directory_lookup(current_dir, token);
		if (current_inode_number == -1) {
			return -1;
		}

		current_inode = &fs->inodes[current_inode_number];
		inode_update_atime(current_inode);
		token = strtok(NULL, "/");
	}

	return current_inode_number;
}

int
fs_resolve_parent(filesystem_t *fs,
                  const char *path,
                  inode_t **parent_inode,
                  char *target_name)
{
	if (!path || !fs || path[0] == '\0') {
		return -1;
	}

	if (strlen(path) >= MAX_PATH_LEN) {
		return -ENAMETOOLONG;
	}

	int len_path = strlen(path);
	int pos_last_dir = -1;
	for (int i = len_path - 1; i >= 0; i--) {
		if (path[i] == '/') {
			pos_last_dir = i;
			break;
		}
	}

	if (pos_last_dir == -1) {
		return -1;
	}

	// Copiamos el target
	int len_name = strlen(path) - (pos_last_dir + 1);
	if (len_name <= 0 || len_name >= MAX_FILENAME_LEN) {
		return -1;
	}
	strncpy(target_name, path + (pos_last_dir + 1), MAX_FILENAME_LEN);
	target_name[MAX_FILENAME_LEN - 1] = '\0';

	// Obtenemos el parent_inode
	char parent_path[MAX_PATH_LEN];
	if (pos_last_dir == 0) {
		strcpy(parent_path, "/");
	} else {
		strncpy(parent_path, path, pos_last_dir);
		parent_path[pos_last_dir] = '\0';
	}
	int parent_inode_number = fs_resolve_path(fs, parent_path);

	if (parent_inode_number == -1) {
		return -1;
	}

	*parent_inode = &fs->inodes[parent_inode_number];
	return 0;
}

int
fs_create_symlink(filesystem_t *fs, const char *target, const char *linkpath)
{
	if (!fs || !target || !linkpath) {
		return -EINVAL;
	}

	inode_t *parent_dir_inode;
	char filename[MAX_FILENAME_LEN];
	if (fs_resolve_parent(fs, linkpath, &parent_dir_inode, filename) < 0) {
		return -ENOENT;
	}

	if (parent_dir_inode->inode_type != INODE_DIR) {
		return -ENOTDIR;
	}

	directory_t *parent_dir = get_directory_by_inode(fs, parent_dir_inode);
	if (!parent_dir) {
		return -EIO;
	}

	if (directory_lookup(parent_dir, filename) >= 0) {
		return -EEXIST;
	}

	int inode_idx = allocate_inode(fs);
	if (inode_idx < 0) {
		return -ENOSPC;
	}
	fs->superblock.free_inodes_count--;

	inode_t *inode = &fs->inodes[inode_idx];
	inode->inode_type = INODE_SYMLINK;
	inode->owner_user_id = getuid();
	inode->owner_group_id = getgid();
	inode->permission_bits = PERM_RWX;
	inode->hard_links_count = 1;

	size_t len = strlen(target);
	if (len >= fs->superblock.max_file_size_bytes) {
		inode_free(fs, inode_idx);
		fs->superblock.free_inodes_count++;
		return -ENAMETOOLONG;
	}
	inode->file_size_bytes = (uint32_t) len;

	int block_free = find_free_block(&fs->bitmap);
	if (block_free < 0 || inode_add_direct_block(fs, inode, block_free) != 0) {
		inode_free(fs, inode_idx);
		fs->superblock.free_inodes_count++;
		return -ENOSPC;
	}
	fs->superblock.free_blocks_count--;

	uint8_t *data = fs->data_block[block_free].data;
	memset(data, 0, BLOCK_SIZE);
	memcpy(data, target, len);

	if (directory_add_entry(parent_dir, filename, inode_idx) != 0) {
		inode_free(fs, inode_idx);
		fs->superblock.free_inodes_count++;
		fs->superblock.free_blocks_count++;
		return -ENOSPC;
	}

	return 0;
}

int
fs_read_symlink(filesystem_t *fs, const char *path, char *buf, size_t size)
{
	if (!fs || !path || !buf) {
		return -EINVAL;
	}

	int inode_idx = fs_resolve_path(fs, path);
	if (inode_idx < 0) {
		return -ENOENT;
	}

	inode_t *inode = &fs->inodes[inode_idx];
	if (inode->inode_type != INODE_SYMLINK) {
		return -EINVAL;
	}

	int block_number = inode_get_direct_block(inode, 0);
	if (block_number < 0) {
		return -EIO;
	}

	uint8_t *data = fs->data_block[block_number].data;
	size_t link_len = inode->file_size_bytes;
	size_t to_copy = (link_len < size - 1) ? link_len : size - 1;
	memcpy(buf, data, to_copy);
	buf[to_copy] = '\0';

	return 0;
}

int
fs_create_hard_link(filesystem_t *fs, const char *from, const char *to)
{
	if (!fs || !from || !to) {
		return -EINVAL;
	}

	int src_inode_num = fs_resolve_path(fs, from);
	if (src_inode_num < 0) {
		return -ENOENT;
	}

	inode_t *src_inode = &fs->inodes[src_inode_num];
	if (src_inode->inode_type == INODE_DIR) {
		return -EPERM;
	}

	inode_t *parent_dir_inode;
	char filename[MAX_FILENAME_LEN];
	if (fs_resolve_parent(fs, to, &parent_dir_inode, filename) < 0) {
		return -ENOENT;
	}

	if (parent_dir_inode->inode_type != INODE_DIR) {
		return -ENOTDIR;
	}

	directory_t *parent_dir = get_directory_by_inode(fs, parent_dir_inode);
	if (!parent_dir) {
		return -EIO;
	}

	if (directory_lookup(parent_dir, filename) >= 0) {
		return -EEXIST;
	}

	if (directory_add_entry(parent_dir, filename, src_inode_num) != 0) {
		return -ENOSPC;
	}

	inode_increment_links(src_inode);

	return 0;
}

int
fs_change_mode(filesystem_t *fs, const char *path, mode_t mode)
{
	if (!fs || !path) {
		return -EINVAL;
	}

	int inode_num = fs_resolve_path(fs, path);
	if (inode_num < 0) {
		return -ENOENT;
	}

	inode_t *inode = &fs->inodes[inode_num];
	uint16_t new_perm = 0;
	if (mode & S_IRUSR || mode & S_IRGRP || mode & S_IROTH) {
		new_perm += PERM_READ;
	}
	if (mode & S_IWUSR || mode & S_IWGRP || mode & S_IWOTH) {
		new_perm += PERM_WRITE;
	}
	if (mode & S_IXUSR || mode & S_IXGRP || mode & S_IXOTH) {
		new_perm += PERM_EXEC;
	}

	inode->permission_bits = new_perm;
	inode_update_ctime(inode);
	return 0;
}

int
fs_change_owner(filesystem_t *fs, const char *path, uid_t uid, gid_t gid)
{
	if (!fs || !path) {
		return -EINVAL;
	}

	int inode_num = fs_resolve_path(fs, path);
	if (inode_num < 0) {
		return -ENOENT;
	}

	inode_t *inode = &fs->inodes[inode_num];
	inode->owner_user_id = uid;
	inode->owner_group_id = gid;
	inode_update_ctime(inode);
	return 0;
}

int
fs_check_permission(filesystem_t *fs, int inode_num, uid_t uid, gid_t gid, int access_mode)
{
	if (!fs || inode_num < 0 || inode_num >= MAX_INODES) {
		return -EINVAL;
	}

	inode_t *inode = &fs->inodes[inode_num];

	int r_bit = inode->permission_bits & PERM_READ;
	int w_bit = inode->permission_bits & PERM_WRITE;
	int x_bit = inode->permission_bits & PERM_EXEC;

	int allowed_read = r_bit ? 1 : 0;
	int allowed_write = w_bit ? 1 : 0;
	int allowed_exec = x_bit ? 1 : 0;

	if ((access_mode & R_OK) && !allowed_read) {
		return -EACCES;
	}
	if ((access_mode & W_OK) && !allowed_write) {
		return -EACCES;
	}
	if ((access_mode & X_OK) && !allowed_exec) {
		return -EACCES;
	}

	return 0;
}