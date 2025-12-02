#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

/**
 * Abre el archivo origen y valida que sea un archivo regular.
 *
 * Retorna el file descriptor del archivo abierto, o -1 en caso de error.
 */
static int
open_and_validate_source(const char *path, struct stat *st)
{
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		perror("open");
		return -1;
	}

	if (fstat(fd, st) < 0) {
		perror("fstat");
		close(fd);
		return -1;
	}

	// Verificar que sea un archivo regular
	if (!S_ISREG(st->st_mode)) {
		fprintf(stderr, "Error: origen no es un archivo regular\n");
		close(fd);
		return -1;
	}

	return fd;
}

/**
 * Crea el archivo destino verificando que no exista previamente.
 *
 * Retorna el file descriptor del archivo creado, o -1 en caso de error.
 */
static int
create_dest(const char *path, mode_t mode)
{
	if (access(path, F_OK) == 0) {
		fprintf(stderr, "Error: el destino ya existe\n");
		return -1;
	}

	// O_EXCL asegura que falle si el archivo existe
	int fd = open(path, O_RDWR | O_CREAT | O_EXCL, mode);
	if (fd < 0) {
		perror("open");
		return -1;
	}

	return fd;
}

/**
 * Copia los datos usando 'mmap' para mapear ambos archivos a memoria.
 *
 * Retorna 0 si la copia fue exitosa, o -1 en caso de error.
 */
static int
copy_data(int src_fd, int dst_fd, off_t size)
{
	// Establecer el tamaño del archivo destino antes de mapearlo
	if (ftruncate(dst_fd, size) < 0) {
		perror("ftruncate");
		return -1;
	}

	// Mapear archivo origen
	void *src = mmap(NULL, size, PROT_READ, MAP_PRIVATE, src_fd, 0);
	if (src == MAP_FAILED) {
		perror("mmap");
		return -1;
	}

	// Mapear archivo destino
	void *dst = mmap(NULL, size, PROT_WRITE, MAP_SHARED, dst_fd, 0);
	if (dst == MAP_FAILED) {
		perror("mmap");
		munmap(src, size);
		return -1;
	}

	memcpy(dst, src, size);

	// Liberar las regiones mapeadas
	munmap(src, size);
	munmap(dst, size);

	return 0;
}

/**
 * Realiza la copia manejando la apertura de archivos, validación, copia de datos y limpieza.
 *
 * Retorna 0 si la copia fue exitosa, o -1 en caso de error.
 */
static int
copy_file(const char *src_path, const char *dst_path)
{
	struct stat st;
	int src_fd = open_and_validate_source(src_path, &st);
	if (src_fd < 0) {
		return -1;
	}

	int dst_fd = create_dest(dst_path, st.st_mode);
	if (dst_fd < 0) {
		close(src_fd);
		return -1;
	}

	int result = 0;
	// Solo copiar si el archivo tiene contenido
	if (st.st_size > 0) {
		result = copy_data(src_fd, dst_fd, st.st_size);
	}

	close(src_fd);
	close(dst_fd);

	if (result < 0) {
		unlink(dst_path);
	}

	return result;
}

int
main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Uso: cp <origen> <destino>\n");
		return EXIT_FAILURE;
	}

	return copy_file(argv[1], argv[2]) < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}