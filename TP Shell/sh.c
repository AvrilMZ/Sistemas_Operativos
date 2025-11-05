#include "defs.h"
#include "types.h"
#include "readline.h"
#include "runcmd.h"

char prompt[PRMTLEN] = { 0 };

// runs a shell command
static void
run_shell()
{
	char *cmd;

	while ((cmd = read_line(prompt)) != NULL)
		if (run_cmd(cmd) == EXIT_SHELL)
			return;
}

// cleans up finished child processes
static void
sigchild_handler()
{
	pid_t pid;
	int status;

	while ((pid = waitpid(0, &status, WNOHANG)) > 0) {
		printf("\n==> terminado: PID=%d\n", pid);
	}
}

// sets the SIGCHLD handler
static void
set_sig_handlers()
{
	struct sigaction sig_act;
	sigemptyset(&sig_act.sa_mask);

	sig_act.sa_handler = sigchild_handler;
	sig_act.sa_flags = SA_RESTART | SA_NOCLDSTOP | SA_ONSTACK;

	if (sigaction(SIGCHLD, &sig_act, NULL) == -1) {
		perror("sigaction");
		_exit(EXIT_FAILURE);
	}
}

// initializes the shell
// with the "HOME" directory
static void
init_shell()
{
	char buf[BUFLEN] = { 0 };
	char *home = getenv("HOME");

	setpgid(0, 0);
	set_sig_handlers();

	if (chdir(home) < 0) {
		snprintf(buf, sizeof buf, "cannot cd to %s ", home);
		perror(buf);
	} else {
		snprintf(prompt, sizeof prompt, "(%s)", home);
	}
}

int
main(void)
{
	init_shell();

	run_shell();

	return 0;
}
