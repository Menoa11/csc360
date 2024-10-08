#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>     
#include "ssi.h"


int shell_printed; // Make sure shell isnt printed out twice

int main() {
    char input[MAX_CMD_LEN];

    // Register the SIGCHLD and SIGINT handler for background process termination and ctrl+c execution
    signal(SIGCHLD, handle_sigchld);
    signal(SIGINT, sigint_handler);

    while (1) {

        //The visual prompt
        if (!shell_printed){
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            printf("%s@%s: %s > ", getlogin(), "hostname", cwd);
        }
        shell_printed = 0;

        // Read input from the user
        fgets(input, sizeof(input), stdin);
    
        // Check for change directory command
        if (strncmp(input, "cd", 2) == 0) {
            char *path = strtok(input + 2, " \n");
            change_directory(path);
        }
        // Check for background command
        else if (strncmp(input, "bg ", 3) == 0) {
            execute_background(input + 3);
            shell_printed = 0;
        }
        // Check for bglist command
        else if (strncmp(input, "bglist", 6) == 0) {
            print_bg_processes();
        }
        // Leave shell when requested
        else if (strncmp(input, "exit", 4) == 0) {
            break;
        }
        else if (strncmp(input, "kill -9", 7) == 0) {
            kill_process(input);
        }
        // Otherwise, execute as a foreground command
        else {
            execute_foreground(input);
        }
    }

    return 0;
}