#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

const int INDICE_NUMERO = 1;
const int INDICE_PIPE_LECTURA = 0;
const int INDICE_PIPE_ESCRITURA = 1;

/**
 * Filtra los múltiplos del primer número encontrado del lado izquierdo en el pipe dado.
 */
void
filtrar_multiplos(int pipe_entrada)
{
	int primo;
	if (read(pipe_entrada, &primo, sizeof(int)) <= 0) {
		return;
	}

	printf("primo %i\n", primo);

	int nuevo_pipe[2];
	if (pipe(nuevo_pipe) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	int process_id = fork();
	if (process_id == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (process_id == 0) {
		close(nuevo_pipe[INDICE_PIPE_ESCRITURA]);
		close(pipe_entrada);

		filtrar_multiplos(nuevo_pipe[INDICE_PIPE_LECTURA]);

		close(nuevo_pipe[INDICE_PIPE_LECTURA]);
		exit(EXIT_SUCCESS);
	} else {
		close(nuevo_pipe[INDICE_PIPE_LECTURA]);

		int numero;
		while (read(pipe_entrada, &numero, sizeof(int)) > 0) {
			if (numero % primo != 0) {
				if (write(nuevo_pipe[INDICE_PIPE_ESCRITURA],
				          &numero,
				          sizeof(int)) == -1) {
					perror("write");
					exit(EXIT_FAILURE);
				}
			}
		}

		close(pipe_entrada);
		close(nuevo_pipe[INDICE_PIPE_ESCRITURA]);
		wait(NULL);
	}
}

/**
 * Genera números desde 2 hasta `max_numero` y los envía al pipe dado.
 */
void
generar_numeros(int pipe_salida, int max_numero)
{
	for (int i = 2; i <= max_numero; i++) {
		if (write(pipe_salida, &i, sizeof(int)) == -1) {
			perror("write");
			exit(EXIT_FAILURE);
		}
	}
}

/**
 * Implementa la criba de Eratóstenes.
 *
 * @param numero: debe ser un número natural mayor o igual a 2.
 */
void
criba_de_eratostenes(int numero)
{
	int pipe_file_descriptor[2];
	if (pipe(pipe_file_descriptor) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	int process_id = fork();
	if (process_id == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (process_id == 0) {
		close(pipe_file_descriptor[INDICE_PIPE_ESCRITURA]);

		filtrar_multiplos(pipe_file_descriptor[INDICE_PIPE_LECTURA]);

		close(pipe_file_descriptor[INDICE_PIPE_LECTURA]);
		exit(EXIT_SUCCESS);
	} else {
		close(pipe_file_descriptor[INDICE_PIPE_LECTURA]);

		generar_numeros(pipe_file_descriptor[INDICE_PIPE_ESCRITURA],
		                numero);

		close(pipe_file_descriptor[INDICE_PIPE_ESCRITURA]);
		wait(NULL);
	}
}

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Uso: %s <número>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int numero = atoi(argv[INDICE_NUMERO]);
	if (numero < 2) {
		fprintf(stderr,
		        "El número ingresado debe ser mayor o igual a 2\n");
		exit(EXIT_FAILURE);
	}

	criba_de_eratostenes(numero);

	return EXIT_SUCCESS;
}