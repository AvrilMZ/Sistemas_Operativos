#ifndef CONFIG_H
#define CONFIG_H

#include <errno.h>
#include <stdio.h>

#define MAX_INODES 128
#define MAX_BLOCKS 1024
#define BLOCK_SIZE 4096

#define MAX_FILENAME_LEN 32
#define MAX_PATH_LEN 256
#define MAX_DIR_DEPTH 16

#define PERM_READ 0x4
#define PERM_WRITE 0x2
#define PERM_EXEC 0x1
#define PERM_RW (PERM_READ | PERM_WRITE)
#define PERM_RWX (PERM_READ | PERM_WRITE | PERM_EXEC)
#define FS_MODE_DIR 0040000
#define FS_MODE_FILE 0100000
#define FS_MODE_SYMLINK 0120000

#define NAME_ROOT "/"

#endif /* CONFIG_H */