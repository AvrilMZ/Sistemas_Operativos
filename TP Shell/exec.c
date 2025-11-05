#include "exec.h"

// sets "key" with the key part of "arg"
// and null-terminates it
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  key = "KEY"
//
static void
get_environ_key(char *arg, char *key)
{
	int i;
	for (i = 0; arg[i] != '='; i++)
		key[i] = arg[i];

	key[i] = END_STRING;
}

// sets "value" with the value part of "arg"
// and null-terminates it
// "idx" should be the index in "arg" where "=" char
// resides
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  value = "value"
//
static void
get_environ_value(char *arg, char *value, int idx)
{
	size_t i, j;
	for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++)
		value[j] = arg[i];

	value[j] = END_STRING;
}

// sets the environment variables received
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
static void
set_environ_vars(char **eargv, int eargc)
{
	for (int i = 0; i < eargc; i++) {
		char *pair_str = eargv[i];
		int idx_equal = block_contains(pair_str, '=');
		if (idx_equal == -1) {
			continue;
		}
		int key_buffer = idx_equal + 1;
		int value_buffer = strlen(pair_str) - idx_equal;
		char key[key_buffer];
		char value[value_buffer];
		get_environ_key(pair_str, key);
		get_environ_value(pair_str, value, idx_equal);
		if (setenv(key, value, 1) == -1) {
			fprintf(stderr, "setenv failed : %s\n", strerror(errno));
		}
	}
}

// opens the file in which the stdin/stdout/stderr
// flow will be redirected, and returns
// the file descriptor
//
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file
static int
open_redir_fd(char *file, int flags)
{
	if (!file) {
		_exit(EXIT_FAILURE);
	}

	int fd = 0;
	if (flags & O_CREAT) {
		fd = open(file, flags, S_IWUSR | S_IRUSR);
	} else {
		fd = open(file, flags);
	}

	if (fd == -1) {
		perror("open");
		_exit(EXIT_FAILURE);
	}

	return fd;
}

// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
// - casting could be a good option
void
exec_cmd(struct cmd *cmd)
{
	struct execcmd *e;
	struct backcmd *b;
	struct execcmd *r;
	struct pipecmd *p;

	switch (cmd->type) {
	// spawns a command
	case EXEC:
		e = (struct execcmd *) cmd;
		set_environ_vars(e->eargv, e->eargc);
		if (execvp(e->argv[0], e->argv) == -1) {
			fprintf(stderr, "exec failed : %s\n", strerror(errno));
			_exit(EXIT_FAILURE);
		}
		break;

	// runs a command in background
	case BACK: {
		b = (struct backcmd *) cmd;
		exec_cmd(b->c);
		break;
	}

	// changes the input/output/stderr flow
	case REDIR: {
		if (strlen(cmd->scmd) == 0) {
			_exit(EXIT_FAILURE);
		}

		r = (struct execcmd *) cmd;

		if (strlen(r->in_file) != 0) {
			int fd = open_redir_fd(r->in_file, O_RDONLY);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}

		if (strlen(r->out_file) != 0) {
			int fd = open_redir_fd(r->out_file,
			                       O_WRONLY | O_CREAT | O_TRUNC);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}

		if (strlen(r->err_file) != 0) {
			if (strcmp(r->err_file, "&1") == 0) {
				dup2(STDOUT_FILENO, STDERR_FILENO);
			} else {
				int fd = open_redir_fd(r->err_file,
				                       O_WRONLY | O_CREAT |
				                               O_TRUNC);
				dup2(fd, STDERR_FILENO);
				close(fd);
			}
		}

		r->type = EXEC;
		exec_cmd((struct cmd *) r);
		break;
	}

	// pipes two commands
	case PIPE: {
		p = (struct pipecmd *) cmd;

		int fd[2];
		if (pipe(fd) == -1) {
			perror("pipe");
			_exit(EXIT_FAILURE);
		}

		pid_t left_pid = fork();
		if (left_pid == 0) {
			close(fd[0]);
			dup2(fd[1], STDOUT_FILENO);
			close(fd[1]);
			exec_cmd(p->leftcmd);
			perror("exec_cmd");
			_exit(EXIT_FAILURE);
		}

		pid_t right_pid = fork();
		if (right_pid == 0) {
			close(fd[1]);
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			exec_cmd(p->rightcmd);
			perror("exec_cmd");
			_exit(EXIT_FAILURE);
		}

		close(fd[0]);
		close(fd[1]);

		waitpid(left_pid, NULL, 0);
		waitpid(right_pid, NULL, 0);

		free_command(parsed_pipe);
		_exit(EXIT_SUCCESS);
		break;
	}
	}
}
