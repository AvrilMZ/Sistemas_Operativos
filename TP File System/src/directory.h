#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stdint.h>
#include "filesystem.h"

typedef struct dir_entry {
	char name[MAX_FILENAME_LEN];  // Nombre del archivo o subdirectorio
	uint32_t inode_number;        // Inodo correspondiente
} dir_entry_t;

typedef struct directory {
	dir_entry_t entries[MAX_BLOCKS];  // Arreglo de entradas
	uint32_t entry_count;             // Cantidad de entradas usadas
} directory_t;

/**
 * Initializes a directory structure.
 *
 * Sets `entry_count` to 0.
 */
void directory_init(directory_t *dir);

/**
 * Searches for an entry by name.
 *
 * Returns the inode number if found, or -1 if not found.
 */
int directory_lookup(directory_t *dir, const char *name);

/**
 * Adds a new entry to the directory.
 *
 * Returns 0 on success, -1 if the directory is full or the name already exists.
 */
int directory_add_entry(directory_t *dir, const char *name, uint32_t inode_number);

/**
 * Removes an entry by name from the directory.
 *
 * Returns 0 on success, -1 if the name does not exist.
 */
int directory_remove_entry(directory_t *dir, const char *name);

/**
 * Prints or returns a list of all entries in the directory.
 */
int directory_list_entries(directory_t *dir, char buffer[][MAX_FILENAME_LEN]);

/**
 * Returns the directory represented by an inode.
 */
directory_t *get_directory_by_inode(filesystem_t *fs, inode_t *inode);

#endif /* DIRECTORY_H */