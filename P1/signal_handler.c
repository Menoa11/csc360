#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <signal.h>
#include <unistd.h>     
#include "ssi.h"

// Signal handler for child process termination
void handle_sigchld(int sig) {
    int status;
    pid_t pid;

    fflush(stdout);
    // Wait for any child process that has terminated
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        bg_process_t *current = bg_head;

        // Search for the terminated process in the background list
        while (current != NULL) {
            if (current->pid == pid) {
                printf("\n");
                printf("%d: %s has terminated.\n", pid, current->command);
                fflush(stdout);
                break;
            }
            current = current->next;
        }

        // Remove the terminated process from the list
        remove_bg_process(pid);
    }
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s@%s: %s > ", getlogin(), "hostname", cwd);
    fflush(stdout);
    shell_printed = 1;
}

// Ctrl+C signal handler
void sigint_handler(int sig) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT); // Send SIGINT to the foreground process
    }
    shell_printed = 1;
}