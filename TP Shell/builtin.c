#include "builtin.h"
#include "utils.h"
#include <unistd.h>
#include <string.h>

char *get_directory(char *cmd);
// returns true if the 'exit' call
// should be performed
//
// (It must not be called from here)
int
exit_shell(char *cmd)
{
	if (strcmp(cmd, "exit") == 0) {
		return 1;
	}

	return 0;
}


char *
get_directory(char *cmd)
{
	if (!cmd) {
		return NULL;
	}

	char *tmp = strchr(cmd, ' ');
	if (tmp == NULL) {
		return NULL;
	}

	size_t p = strlen(cmd);
	char *directory = malloc(p - (tmp - cmd) + 1);
	int posicion = (tmp - cmd) + 1;
	int i = 0;
	while (cmd[posicion] != '\0') {
		directory[i] = cmd[posicion];
		i++;
		posicion++;
	}
	directory[i] = '\0';

	return directory;
}
// returns true if "chdir" was performed
//  this means that if 'cmd' contains:
// 	1. $ cd directory (change to 'directory')
// 	2. $ cd (change to $HOME)
//  it has to be executed and then return true
//
//  Remember to update the 'prompt' with the
//  	new directory.
//
// Examples:
//  1. cmd = ['c','d', ' ', '/', 'b', 'i', 'n', '\0']
//  2. cmd = ['c','d', '\0']
int
cd(char *cmd)
{
	if (!cmd || strncmp(cmd, "cd", 2) != 0 ||
	    (strlen(cmd) > 2 && cmd[2] != ' ')) {
		return 0;
	}

	char *directory = get_directory(cmd);
	if (directory == NULL) {
		char *home = getenv("HOME");
		if (chdir(home) == -1) {
			fprintf(stderr, "cd failed : %s\n", strerror(errno));
		}
	} else {
		int tmp = chdir(directory);
		if (tmp == -1 && errno == ENOENT) {
			fprintf(stderr,
			        "cannot cd to %s : %s\n",
			        directory,
			        strerror(errno));
		} else if (tmp == -1) {
			fprintf(stderr, "cd failed : %s\n", strerror(errno));
		}
		free(directory);
	}
	if (getcwd(prompt, sizeof(prompt)) == NULL) {
		perror("getcwd");
	}
	return 1;
}

// returns true if 'pwd' was invoked
// in the command line
//
// (It has to be executed here and then
// 	return true)
int
pwd(char *cmd)
{
	if (strcmp(cmd, "pwd") == 0) {
		char path[PRMTLEN];
		if (getcwd(path, sizeof(path)) == NULL) {
			fprintf(stderr, "pwd failed : %s\n", strerror(errno));
		}
		strcat(path, "\n");
		printf("%s", path);
		return 1;
	}
	return 0;
}

// returns true if `history` was invoked
// in the command line
//
// (It has to be executed here and then
// 	return true)
int
history(char *cmd)
{
	// Your code here

	return 0;
}
