#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ssi.h"

pid_t foreground_pid = -1;

// Execute a command in the foreground
void execute_foreground(char *input) {
    char *args[64];
    char *token = strtok(input, " \n");
    int i = 0;

    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;

    pid_t pid = fork();

    if (pid == 0) {
        // Child process executes the command
        if (execvp(args[0], args) == -1) {
            perror("Error executing command");
        }
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process: set the foreground process ID
        foreground_pid = pid;

        // Wait for the foreground process to finish
        int status;
        waitpid(pid, &status, 0);

        // Reset the foreground process ID
        foreground_pid = -1;
    } else {
        perror("Fork failed");
    }
}