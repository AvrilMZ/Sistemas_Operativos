#ifndef NARGS
#define NARGS 4
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

/**
 * Ejecuta el `comando` con los argumentos dados.
 */
void
ejecutar_comando(char *comando, char **argumentos, int num_args)
{
	char **argv = calloc(num_args + 2, sizeof(char *));
	if (!argv) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	argv[0] = comando;
	for (int i = 0; i < num_args; i++) {
		argv[i + 1] = argumentos[i];
	}
	argv[num_args + 1] = NULL;

	int process_id = fork();
	if (process_id == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (process_id == 0) {
		if (execvp(comando, argv) == -1) {
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	} else {
		wait(NULL);
	}

	free(argv);
}

/**
 * Se libera la memoria de cada cadena en el arreglo `copia`.
 */
void
liberar_copia(char **copia, int num_args)
{
	for (int i = 0; i < num_args; i++) {
		free(copia[i]);
	}
}

/**
 * Ejecuta el `comando` dado cada vez que se acumulan `NARGS` argumentos o al final si son menos.
 */
void
procesar_entrada(char *comando)
{
	char **buffer_args = calloc(NARGS, sizeof(char *));
	if (!buffer_args) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	char *linea = NULL;
	size_t len = 0;
	ssize_t caracteres_leidos;
	int contador_args = 0;

	while ((caracteres_leidos = getline(&linea, &len, stdin)) != -1) {
		if (linea[caracteres_leidos - 1] == '\n') {
			linea[caracteres_leidos - 1] = '\0';
		}

		buffer_args[contador_args] = strdup(linea);
		if (!buffer_args[contador_args]) {
			perror("strdup");
			exit(EXIT_FAILURE);
		}

		contador_args++;

		if (contador_args == NARGS) {
			ejecutar_comando(comando, buffer_args, contador_args);
			liberar_copia(buffer_args, contador_args);
			contador_args = 0;
		}
	}

	if (contador_args > 0) {
		ejecutar_comando(comando, buffer_args, contador_args);
		liberar_copia(buffer_args, contador_args);
	}

	free(linea);
	free(buffer_args);
}

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Uso: %s <comando>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	char *comando = argv[1];
	procesar_entrada(comando);

	return EXIT_SUCCESS;
}
