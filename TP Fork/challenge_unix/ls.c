#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

struct file_info {
	char type;
	unsigned int perms;
	unsigned int uid;
	char name[256];
	char link_target[256];
	int is_link;
};

/**
 * Devuelve el símbolo de tipo de archivo.
 *
 * Retorna un carácter que representa el tipo de archivo.
 */
static char
get_file_type_symbol(mode_t mode)
{
	if (S_ISREG(mode)) {
		return '-';
	} else if (S_ISDIR(mode)) {
		return 'd';
	} else if (S_ISLNK(mode)) {
		return 'l';
	} else if (S_ISCHR(mode)) {
		return 'c';
	} else if (S_ISBLK(mode)) {
		return 'b';
	} else if (S_ISFIFO(mode)) {
		return 'p';
	} else if (S_ISSOCK(mode)) {
		return 's';
	}
	return '?';
}

/**
 * Lee la información de un archivo desde el filesystem.
 *
 * Retorna 0 si tuvo éxito, -1 en caso de error.
 */
static int
read_file_info(const char *dirpath, const char *name, struct file_info *info)
{
	char path[4096];
	snprintf(path, sizeof(path), "%s/%s", dirpath, name);

	struct stat st;
	if (lstat(path, &st) < 0) {  // 'lstat' para saber si es un symlink
		perror("lstat");
		return -1;
	}

	info->type = get_file_type_symbol(st.st_mode);
	info->perms = st.st_mode & 0777U;
	info->uid = (unsigned int) st.st_uid;
	strncpy(info->name, name, sizeof(info->name) - 1);
	info->name[sizeof(info->name) - 1] = '\0';
	info->is_link = S_ISLNK(st.st_mode);
	if (info->is_link) {
		ssize_t len = readlink(path,
		                       info->link_target,
		                       sizeof(info->link_target) - 1);
		if (len >= 0) {
			info->link_target[len] = '\0';
		} else {
			info->link_target[0] = '\0';
		}
	}

	return 0;
}

/**
 * Imprime la información de un archivo.
 */
static void
print_file_info(const struct file_info *info)
{
	printf("%c %03o %u %s", info->type, info->perms, info->uid, info->name);
	if (info->is_link && info->link_target[0] != '\0') {
		printf(" -> %s", info->link_target);
	}
	putchar('\n');
}

/**
 * Lista todas las entradas de un directorio y muestra su información.
 *
 * Retorna 0 si tuvo éxito, o 1 si no se pudo abrir el directorio.
 */
static int
list_dir(const char *path)
{
	DIR *dir = opendir(path);
	if (!dir) {
		perror("opendir");
		return 1;
	}

	struct dirent *entry;
	while ((entry = readdir(dir))) {
		struct file_info info;
		if (read_file_info(path, entry->d_name, &info) == 0) {
			print_file_info(&info);
		}
	}

	closedir(dir);
	return 0;
}

int
main(int argc, char *argv[])
{
	if (argc == 1) {
		return list_dir(".");
	}

	for (int i = 1; i < argc; i++) {
		if (argc > 2) {
			printf("%s:\n", argv[i]);
		}
		list_dir(argv[i]);
		if (argc > 2 && i + 1 < argc) {
			putchar('\n');
		}
	}

	return EXIT_SUCCESS;
}