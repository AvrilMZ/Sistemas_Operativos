#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"
#include "inode.h"
#include "bitmap.h"
#include "data_block.h"
#include "superblock.h"

typedef struct filesystem {
	inode_t inodes[MAX_INODES];  // array fijo de inodos
	bitmap_t bitmap;             // inode + data
	superblock_t superblock;
	data_block_t data_block[MAX_BLOCKS];
} filesystem_t;

/**
 * Initializes the entire filesystem:
 * - superblock
 * - bitmaps
 * - table of inodes
 * - root directory
 */
void fs_init(filesystem_t *fs);

/**
 * Creates a new empty file at the given path.
 *
 * Allocates an inode, assigns it a block if needed, and adds an entry in the
 * parent directory. Returns 0 on success or a negative error on failure.
 */
int fs_create_file(filesystem_t *fs, const char *path, mode_t mode);

/**
 * Reads 'size' bytes from the file at 'path', starting at 'offset'.
 *
 * Returns number of bytes read or a negative error on failure.
 */
int fs_read_file(filesystem_t *fs,
                 const char *path,
                 char *buffer,
                 size_t size,
                 off_t offset);

/**
 * Writes 'size' bytes to the file at 'path', starting at 'offset'.
 *
 * Returns number of bytes written or a negative error on failure.
 */
int fs_write_file(filesystem_t *fs,
                  const char *path,
                  const char *buffer,
                  size_t size,
                  off_t offset,
                  int append);

/**
 * Deletes the file at 'path':
 * - Frees all blocks
 * - Frees the inode
 * - Removes the entry from the parent directory
 */
int fs_delete_file(filesystem_t *fs, const char *path);

/**
 * Creates a directory at 'path'.
 *
 * Allocates an inode, initializes '.' and '..' entries, adds it to the parent directory.
 */
int fs_create_directory(filesystem_t *fs, const char *path, mode_t mode, char *name);

/**
 * Lists the content of the directory at 'path'.
 *
 * Returns number of entries or a negative error on failure.
 */
int fs_list_directory(filesystem_t *fs,
                      const char *path,
                      char buffer[][MAX_FILENAME_LEN]);

/**
 * Removes a directory at 'path'.
 *
 * Directory must be empty.
 * Frees inode and updates parent directory.
 */
int fs_remove_directory(filesystem_t *fs, const char *path);

/**
 * Retrieves inode information for 'path'.
 *
 * Fills a fs_stats_t.
 */
int fs_statistics(filesystem_t *fs, const char *path, struct stat *st);

/**
 * Resolves a path string to its inode number.
 *
 * Handles absolute and relative paths.
 * Returns inode number or a negative error on failure.
 */
int fs_resolve_path(filesystem_t *fs, const char *path);

/**
 * Gets the parent directory inode of a path and the name of the target.
 */
int fs_resolve_parent(filesystem_t *fs,
                      const char *path,
                      inode_t **parent_inode,
                      char *target_name);

/**
 * Creates a symbolic link at 'linkpath' pointing to 'target'.
 *
 * Returns 0 on success or a negative error on failure.
 */
int fs_create_symlink(filesystem_t *fs, const char *target, const char *linkpath);

/**
 * Reads the contents of the symbolic link at 'path' into 'buf'.
 *
 * Returns 0 on success or a negative error on failure.
 */
int fs_read_symlink(filesystem_t *fs, const char *path, char *buf, size_t size);

/**
 * Creates a hard link from 'from' to 'to'.
 *
 * Returns 0 on success or a negative error on failure.
 */
int fs_create_hard_link(filesystem_t *fs, const char *from, const char *to);

/**
 * Changes the permission bits of the inode at 'path'.
 *
 * Returns 0 on success or a negative error on failure.
 */
int fs_change_mode(filesystem_t *fs, const char *path, mode_t mode);

/**
 * Changes the owner (uid) and group (gid) of the inode at 'path'.
 *
 * Returns 0 on success or a negative error on failure.
 */
int fs_change_owner(filesystem_t *fs, const char *path, uid_t uid, gid_t gid);

/**
 * Checks permissions for inode 'inode_num' against (uid, gid) and 'access_mode'.
 *
 * Returns 0 if access is allowed or a negative error otherwise.
 */
int fs_check_permission(
        filesystem_t *fs, int inode_num, uid_t uid, gid_t gid, int access_mode);

#endif /* FILESYSTEM_H */
