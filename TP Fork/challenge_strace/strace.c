#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include "syscallent.h"


/**
 * Imprime un mensaje de uso simple.
 *
 * La usamos cuando el usuario no pasa un programa a trazar.
 */
static void
usage(const char *prog_name)
{
	fprintf(stderr, "Uso: %s comando [args...]\n", prog_name);
	exit(EXIT_FAILURE);
}


/**
 * Recibe el pid del proceso hijo, que ya se encuentra bajo 'ptrace'.
 *
 * Mientras el hijo no termine, avanzamos de entrada en entrada y de salida en
 * salida de cada syscall, imprimiendo su nombre y valor de retorno en las salidas.
 */
static void
trace_child(pid_t child_pid)
{
	int status;
	int in_syscall = 0;  // ¿Entrando (1) o saliendo (0) de una syscall?

	// Esperamos la primera parada del hijo
	if (waitpid(child_pid, &status, 0) == -1) {
		perror("waitpid");
		return;
	}

	// Mientras el hijo no termine:
	while (!WIFEXITED(status) && !WIFSIGNALED(status)) {
		struct user_regs_struct regs;

		/**
		 * El hijo sigue hasta la próxima entrada o salida de una
		 * syscall. Cada vez que el hijo se detiene, volvemos al padre.
		 */
		if (ptrace(PTRACE_SYSCALL, child_pid, 0, 0) == -1) {
			perror("ptrace(PTRACE_SYSCALL)");
			break;
		}

		// Esperamos a que el hijo se detenga en la siguiente syscall.
		if (waitpid(child_pid, &status, 0) == -1) {
			perror("waitpid");
			break;
		}

		// Si el hijo terminó salimos del bucle.
		if (WIFEXITED(status) || WIFSIGNALED(status)) {
			break;
		}

		// Obtenemos los registros del hijo.
		if (ptrace(PTRACE_GETREGS, child_pid, 0, &regs) == -1) {
			perror("ptrace(PTRACE_GETREGS)");
			break;
		}

		if (!in_syscall) {
			// Hubo una entrada a una syscall.
			in_syscall = 1;
		} else {
			// Hubo una salida de la syscall.
			long syscall_nr = (long) regs.orig_rax;
			long retval = (long) regs.rax;
			const char *name = get_syscall_name(syscall_nr);
			printf("%s = %ld\n", name, retval);
			in_syscall = 0;  // Para la próxima entrada.
		}
	}
}


int
main(int argc, char *argv[])
{
	if (argc < 2) {
		usage(argv[0]);
	}

	pid_t child = fork();
	if (child == -1) {
		perror("fork");
		return EXIT_FAILURE;
	}

	if (child == 0) {
		// Se marca el proceso hijo como trazable y se ejecuta el comando pedido.
		if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1) {
			perror("ptrace(PTRACE_TRACEME)");
			_exit(EXIT_FAILURE);
		}
		execvp(argv[1], &argv[1]);  // Ejecutamos el programa a trazar.
		perror("execvp");
		_exit(EXIT_FAILURE);
	} else {
		// El proceso padre traza al hijo.
		trace_child(child);
	}

	return EXIT_SUCCESS;
}