#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include <stdint.h>

typedef struct superblock {
	uint32_t total_inodes;         // Total de inodos
	uint32_t total_blocks;         // Total de bloques de datos
	uint32_t free_inodes_count;    // Cantidad de inodos libres
	uint32_t free_blocks_count;    // Cantidad de bloques libres
	uint32_t block_size_bytes;     // Tamaño de cada bloque
	uint32_t max_file_size_bytes;  // Tamaño máximo de archivo soportado
	uint32_t max_filename_length;  // Longitud máxima de un nombre de archivo
	uint32_t max_path_length;      // Longitud máxima de la ruta completa
	uint32_t root_inode_number;    // Número de inodo de la raíz del FS
} superblock_t;

#endif /* SUPERBLOCK_H */