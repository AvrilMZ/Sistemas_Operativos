#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "src/filesystem.h"
#include "src/inode.h"

#define DEFAULT_FILE_DISK "persistence_file.fisopfs"

// absolute path for persistence file used in
// `.init` and `.destroy` FUSE operations
static char filedisk_path[2 * MAX_PATH_LEN];

static int
empty_file(FILE *file)
{
	fseek(file, 0, SEEK_END);
	if (ftell(file) == 0) {  // file empty
		return 1;
	}
	fseek(file, 0, SEEK_SET);
	return 0;
}

filesystem_t *
new_fs()
{
	filesystem_t *fs = calloc(1, sizeof(filesystem_t));
	if (!fs) {
		fprintf(stderr, "Error al crear un nuevo File System.\n");
		return NULL;
	}
	printf("Inicializando nuevo File System.\n");
	fs_init(fs);
	return fs;
}

void *
fisops_init(struct fuse_conn_info *conn)
{
	printf("[DEBUG] fisops init -----------\n");
	FILE *file = fopen(filedisk_path, "rb");

	if (!file) {
		printf("[DEBUG] El archivo de persistencia del File System no "
		       "existe.\n");
		return new_fs();
	}

	if (empty_file(file) > 0) {
		printf("[DEBUG]El archivo de persistencia del File System está "
		       "vacío.\n");
		fclose(file);
		return new_fs();
	}

	filesystem_t *fs = calloc(1, sizeof(filesystem_t));
	fs_init(fs);

	if (!fs) {
		fprintf(stderr, "[DEBUG]Error en memoria para inicializar el File System.\n");
		fclose(file);
		return NULL;
	}

	if (fread(fs, sizeof(filesystem_t), 1, file) != 1) {
		fprintf(stderr, "[DEBUG]Error al leer el archivo de persistencia del File System.\n");
		fclose(file);
		free(fs);
		return NULL;
	}

	printf("[DEBUG] File System cargado correctamente.\n");
	fclose(file);
	return fs;
}

static void
fisops_destroy(void *private_data)
{
	if (!private_data) {
		return;
	}

	FILE *file = fopen(filedisk_path, "wb");
	if (!file) {
		fprintf(stderr, "No se pudo persistir el File System en disco.\n");
		free(private_data);
		return;
	}

	filesystem_t *fs = private_data;
	if (fwrite(fs, sizeof(filesystem_t), 1, file) != 1) {
		fprintf(stderr, "Error al desmontar el File System.\n");
		fclose(file);
		free(fs);
		return;
	}

	printf("File System persistido y desmontado exitosamente.\n");
	fclose(file);
	free(fs);
}

static int
fisops_flush(const char *path, struct fuse_file_info *info)
{
	(void) path;
	(void) info;
	return 0;
}

static int
fisopfs_getattr(const char *path, struct stat *st)
{
	printf("[debug] fisopfs_getattr - path: %s\n", path);

	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	int res = fs_statistics(fs, path, st);
	if (res < 0) {
		return res;
	}

	return 0;
}

static int
fisopfs_readdir(const char *path,
                void *buffer,
                fuse_fill_dir_t filler,
                off_t offset,
                struct fuse_file_info *fi)
{
	printf("[debug] fisopfs_readdir - path: %s\n", path);

	// pseudo directories '.' y '..'
	filler(buffer, ".", NULL, 0);
	filler(buffer, "..", NULL, 0);

	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	struct fuse_context *ctx = fuse_get_context();
	uid_t uid = ctx->uid;
	gid_t gid = ctx->gid;

	int inode_num = fs_resolve_path(fs, path);
	if (inode_num < 0) {
		return -ENOENT;
	}

	int err = fs_check_permission(fs, inode_num, uid, gid, R_OK);
	if (err < 0) {
		return err;
	}

	char entries[MAX_INODES][MAX_FILENAME_LEN];

	int count = fs_list_directory(fs, path, entries);
	if (count < 0) {
		return -ENOENT;
	}

	for (int i = 0; i < count; i++) {
		filler(buffer, entries[i], NULL, 0);
	}

	return 0;
}

static int
fisopfs_read(const char *path,
             char *buffer,
             size_t size,
             off_t offset,
             struct fuse_file_info *fi)
{
	printf("[debug] fisopfs_read - path: %s, offset: %lu, size: %lu\n",
	       path,
	       offset,
	       size);

	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	struct fuse_context *ctx = fuse_get_context();
	uid_t uid = ctx->uid;
	gid_t gid = ctx->gid;

	int inode_num = fs_resolve_path(fs, path);
	if (inode_num < 0) {
		return -ENOENT;
	}

	int err = fs_check_permission(fs, inode_num, uid, gid, R_OK);
	if (err < 0) {
		return err;
	}

	int bytes_read = fs_read_file(fs, path, buffer, size, offset);
	if (bytes_read < 0) {
		return -ENOENT;
	}

	return bytes_read;
}

static int
fisopfs_write(const char *path,
              const char *buf,
              size_t size,
              off_t offset,
              struct fuse_file_info *fi)
{
	printf("[DEBUG] FISOP WRITE\n");
	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	struct fuse_context *ctx = fuse_get_context();
	uid_t uid = ctx->uid;
	gid_t gid = ctx->gid;

	int inode_num = fs_resolve_path(fs, path);
	if (inode_num < 0) {
		return -ENOENT;
	}

	int err = fs_check_permission(fs, inode_num, uid, gid, R_OK);
	if (err < 0) {
		return err;
	}

	return fs_write_file(fs, path, buf, size, offset, 0);
}

static int
fisopfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	printf("[DEBUG] FISOP CREATE\n");
	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	return fs_create_file(fs, path, mode);
}

static int
fisopfs_unlink(const char *path)
{
	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	struct fuse_context *ctx = fuse_get_context();
	uid_t uid = ctx->uid;
	gid_t gid = ctx->gid;

	int inode_num = fs_resolve_path(fs, path);
	if (inode_num < 0) {
		return -ENOENT;
	}

	int err = fs_check_permission(fs, inode_num, uid, gid, W_OK);
	if (err < 0) {
		return err;
	}

	return fs_delete_file(fs, path);
}

static int
fisopfs_mkdir(const char *path, mode_t mode)
{
	printf("[DEBUG] fisopfs_mkdir - path: %s\n", path);

	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	inode_t *parent_dir_inode;
	char name[MAX_FILENAME_LEN];
	if (fs_resolve_parent(fs, path, &parent_dir_inode, name) < 0) {
		return -ENOENT;
	}

	struct fuse_context *ctx = fuse_get_context();
	uid_t uid = ctx->uid;
	gid_t gid = ctx->gid;
	int parent_inode_num = (int) (parent_dir_inode - fs->inodes);

	int err = fs_check_permission(fs, parent_inode_num, uid, gid, W_OK);
	if (err < 0) {
		return err;
	}

	int res = fs_create_directory(fs, path, mode, name);
	if (res < 0) {
		return res;
	}

	return 0;
}

static int
fisopfs_rmdir(const char *path)
{
	printf("[DEBUG] fisopfs_rmdir - path: %s\n", path);

	filesystem_t *fs = fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	inode_t *parent_dir_inode;
	char name[MAX_FILENAME_LEN];
	if (fs_resolve_parent(fs, path, &parent_dir_inode, name) < 0) {
		return -ENOENT;
	}

	struct fuse_context *ctx = fuse_get_context();
	uid_t uid = ctx->uid;
	gid_t gid = ctx->gid;
	int parent_inode_num = (int) (parent_dir_inode - fs->inodes);

	int err = fs_check_permission(fs, parent_inode_num, uid, gid, W_OK);
	if (err < 0) {
		return err;
	}

	int res = fs_remove_directory(fs, path);
	if (res < 0) {
		return res;
	}

	return 0;
}

static int
fisopfs_truncate(const char *path, off_t size)
{
	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	struct fuse_context *ctx = fuse_get_context();
	uid_t uid = ctx->uid;
	gid_t gid = ctx->gid;

	int inode_num = fs_resolve_path(fs, path);
	if (inode_num < 0) {
		return -ENOENT;
	}

	int err = fs_check_permission(fs, inode_num, uid, gid, W_OK);
	if (err < 0) {
		return err;
	}

	inode_t *inode = &fs->inodes[inode_num];
	if (size == 0) {
		for (int i = 0; i < NUM_DIRECT_BLOCKS; i++) {
			int blk = inode_get_direct_block(inode, i);
			if (blk != -1) {
				clear_block_used(&fs->bitmap, blk);
				memset(fs->data_block[blk].data, 0, BLOCK_SIZE);
				fs->superblock.free_blocks_count++;
				inode->direct_block_ptrs[i] = -1;
			}
		}
	}

	inode->file_size_bytes = size;
	inode_update_mtime(inode);
	inode_update_ctime(inode);
	return 0;
}

static int
fisopfs_utimens(const char *path, const struct timespec tv[2])
{
	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	int inode_num = fs_resolve_path(fs, path);
	if (inode_num < 0) {
		return -ENOENT;
	}

	inode_t *inode = &fs->inodes[inode_num];
	inode->last_access_time = tv[0].tv_sec;
	inode->last_modification_time = tv[1].tv_sec;
	return 0;
}

static int
fisopfs_symlink(const char *target, const char *linkpath)
{
	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	return fs_create_symlink(fs, target, linkpath);
}

static int
fisopfs_readlink(const char *path, char *buf, size_t size)
{
	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	int res = fs_read_symlink(fs, path, buf, size);
	if (res < 0) {
		return res;
	}

	return 0;
}

static int
fisopfs_link(const char *from, const char *to)
{
	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	return fs_create_hard_link(fs, from, to);
}

static int
fisopfs_chmod(const char *path, mode_t mode)
{
	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	return fs_change_mode(fs, path, mode);
}

static int
fisopfs_chown(const char *path, uid_t uid, gid_t gid)
{
	filesystem_t *fs = (filesystem_t *) fuse_get_context()->private_data;
	if (!fs) {
		return -EIO;
	}

	return fs_change_owner(fs, path, uid, gid);
}

static struct fuse_operations operations = { .getattr = fisopfs_getattr,
	                                     .readdir = fisopfs_readdir,
	                                     .read = fisopfs_read,
	                                     .write = fisopfs_write,
	                                     .create = fisopfs_create,
	                                     .unlink = fisopfs_unlink,
	                                     .mkdir = fisopfs_mkdir,
	                                     .rmdir = fisopfs_rmdir,
	                                     .init = fisops_init,
	                                     .destroy = fisops_destroy,
	                                     .flush = fisops_flush,
	                                     .truncate = fisopfs_truncate,
	                                     .utimens = fisopfs_utimens,
	                                     .symlink = fisopfs_symlink,
	                                     .readlink = fisopfs_readlink,
	                                     .link = fisopfs_link,
	                                     .chmod = fisopfs_chmod,
	                                     .chown = fisopfs_chown };

int
main(int argc, char *argv[])
{
	char *filedisk_name = DEFAULT_FILE_DISK;

	for (int i = 1; i < argc - 1; i++) {
		if (strcmp(argv[i], "--filedisk") == 0) {
			filedisk_name = argv[i + 1];

			// we remove the argument so that FUSE doesn't use our
			// argument or name as folder.
			//
			// equivalent to a pop.
			for (int j = i; j < argc - 1; j++) {
				argv[j] = argv[j + 2];
			}

			argc = argc - 2;
			break;
		}
	}

	// handle absolute path for persistence file
	// so background executions can work properly
	char cwd[MAX_PATH_LEN];
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("getcwd");
		return 1;
	}
	snprintf(filedisk_path, sizeof(filedisk_path), "%s/%s", cwd, filedisk_name);
	return fuse_main(argc, argv, &operations, NULL);
}