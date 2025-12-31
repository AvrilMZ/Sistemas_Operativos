#ifndef INODE_H
#define INODE_H

#include <stdint.h>
#include <time.h>

#define NUM_DIRECT_BLOCKS 12

typedef struct filesystem filesystem_t;
typedef enum { INODE_FILE, INODE_DIR, INODE_SYMLINK } InodeType;

typedef struct inode {
	uint32_t file_size_bytes;  // Tamaño del archivo
	uint16_t owner_user_id;    // ID del propietario
	uint16_t owner_group_id;   // ID del grupo
	uint16_t permission_bits;  // Permisos (lectura, escritura, ejecución)
	time_t last_access_time;   // Último acceso
	time_t last_modification_time;     // Última modificación
	time_t last_metadata_change_time;  // Último cambio del inodo
	uint16_t hard_links_count;         // Cantidad de hard links
	uint32_t direct_block_ptrs[NUM_DIRECT_BLOCKS];  // Punteros a bloques directos
	InodeType inode_type;
} inode_t;

/**
 * Initializes an inode structure with default values, assuming that the inode will be an INODE_FILE.
 */
void inode_init(inode_t *inode);

/**
 * Allocates a new inode in the inode table.
 *
 * Returns the inode number, or -1 if no free inodes are available.
 */
int allocate_inode(filesystem_t *fs);

/**
 * Updates the last access time of the inode to the current time.
 */
void inode_update_atime(inode_t *inode);

/**
 * Updates the last modification time of the inode to the current time.
 */
void inode_update_mtime(inode_t *inode);

/**
 * Updates the last metadata change time of the inode to the current time.
 */
void inode_update_ctime(inode_t *inode);

/**
 * Increments the hard links count of the inode.
 */
void inode_increment_links(inode_t *inode);

/**
 * Decrements the hard links count of the inode.
 */
void inode_decrement_links(inode_t *inode);

/**
 * Adds a direct block pointer to the inode.
 *
 * Returns 0 on success, -1 if no direct block slots are free.
 */
int inode_add_direct_block(filesystem_t *fs, inode_t *inode, uint32_t block_number);

/**
 * Gets the block number at the given direct pointer index.
 *
 * Returns -1 if the index is invalid or the pointer is unused.
 */
int inode_get_direct_block(inode_t *inode, int index);

/**
 * Frees all blocks referenced by the inode and resets the inode to default values.
 */
void inode_free(filesystem_t *fs, int inode_index);

#endif /* INODE_H */