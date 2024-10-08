#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>     
#include <stdlib.h>
#include <errno.h>
#include "ssi.h"

void kill_process(char *input) {
    // Ensure the command starts with "kill -9"
    if (strncmp(input, "kill -9", 7) != 0) {
        printf("Invalid kill command. Use: kill -9 <pid>\n");
        return;
    }

    // Get the PID from the input
    char *pid_str = strtok(input + 7, " \n"); // Skip "kill -9"
    if (pid_str == NULL) {
        printf("Usage: kill -9 <pid>\n");
        return;
    }

    // Convert the pid_str to an integer
    pid_t pid = atoi(pid_str);
    if (pid <= 0) {
        printf("Invalid PID: %s\n", pid_str);
        return;
    }

    // Send the SIGKILL signal to the process
    if (kill(pid, SIGKILL) == -1) {
        // Handle errors
        if (errno == ESRCH) {
            printf("No such process with PID: %d\n", pid);
        } else if (errno == EPERM) {
            printf("Permission denied to kill process with PID: %d\n", pid);
        } else {
            perror("Error killing process");
        }
    } else {
        printf("Process %d has been killed.", pid);
        fflush(stdout);
    }
    shell_printed = 1;
}
