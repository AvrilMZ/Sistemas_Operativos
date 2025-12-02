#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

struct process_info {
	int pid;
	char comm[256];
};

/**
 * Verifica si una cadena representa un número (PID).
 *
 * Retorna 1 si todos los caracteres son dígitos, 0 en caso contrario.
 */
static int
is_numeric(const char *str)
{
	for (int i = 0; str[i]; i++) {
		if (!isdigit(str[i])) {
			return 0;
		}
	}
	return 1;
}

/**
 * Obtiene el PID y el nombre del comando desde '/proc/[pid]/comm'.
 *
 * Retorna 0 si tuvo éxito, -1 en caso de error.
 */
static int
read_process_info(const char *pid_str, struct process_info *info)
{
	info->pid = atoi(pid_str);

	char path[512];
	snprintf(path, sizeof(path), "/proc/%s/comm", pid_str);

	FILE *f = fopen(path, "r");
	if (!f) {
		return -1;
	}

	if (!fgets(info->comm, sizeof(info->comm), f)) {
		fclose(f);
		return -1;
	}

	// Sacar el salto de línea del final
	size_t len = strlen(info->comm);
	if (len > 0 && info->comm[len - 1] == '\n') {
		info->comm[len - 1] = '\0';
	}

	fclose(f);
	return 0;
}

/**
 * Imprime la información de un proceso.
 */
static void
print_process(const struct process_info *info)
{
	printf("%6d %s\n", info->pid, info->comm);
}

/**
 * Lista todos los procesos del sistema leyendo '/proc', mostrando el PID y el
 * comando de cada proceso.
 *
 * Retorna 0 si tuvo éxito, -1 en caso de error.
 */
static int
list_processes(void)
{
	DIR *dir = opendir("/proc");
	if (!dir) {
		perror("opendir");
		return -1;
	}

	printf("%6s %s\n", "PID", "COMMAND");

	struct dirent *entry;
	while ((entry = readdir(dir))) {
		// Solo procesar entradas que sean PIDs
		if (!is_numeric(entry->d_name)) {
			continue;
		}

		struct process_info info;
		if (read_process_info(entry->d_name, &info) == 0) {
			print_process(&info);
		}
	}

	closedir(dir);
	return 0;
}

int
main(void)
{
	return list_processes();
}