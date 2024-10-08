#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "ssi.h"

// Head of the background process list
bg_process_t *bg_head = NULL;

// Execute a command in the background
void execute_background(char *input) {
    char *args[64];
    char command_copy[MAX_CMD_LEN];

    strncpy(command_copy, input, MAX_CMD_LEN - 1); // Create a copy of the command with parameters
    command_copy[MAX_CMD_LEN - 1] = '\0'; // Ensure null-termination
    
    // Remove trailing newline character if present to make sure print commands are on one line
    size_t len = strlen(command_copy);
    if (len > 0 && command_copy[len - 1] == '\n') {
        command_copy[len - 1] = '\0';
    }
    
    char *token = strtok(input, " \n");
    int i = 0;

    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;

    pid_t pid = fork();

    if (pid == 0) {
        // Child process

        // Detach the background process (good practice)
        setsid(); 
        fclose(stdin); 
        fclose(stdout); 
        fclose(stderr); 

        // Execute the command
        if (execvp(args[0], args) == -1) {
            perror("Error executing background command");
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        // Parent process: Add the background process to the linked list
        add_bg_process(pid, command_copy); // Store full command with parameters
        printf("Started background process %d: %s\n", pid, command_copy);
    } else {
        perror("Fork failed");
    }
}


// Add a process to the background process list
void add_bg_process(int pid, char *command) {
    bg_process_t *new_process = malloc(sizeof(bg_process_t));
    if (!new_process) {
        perror("Failed to allocate memory for background process");
        return;
    }
    new_process->pid = pid;
    strcpy(new_process->command, command);
    new_process->next = bg_head;
    bg_head = new_process;
}

// Remove a process from the background list by PID
void remove_bg_process(int pid) {
    bg_process_t *prev = NULL;
    bg_process_t *current = bg_head;

    while (current != NULL && current->pid != pid) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) return; // PID not found

    if (prev == NULL) {
        bg_head = current->next;
    } else {
        prev->next = current->next;
    }

    free(current); // Free the removed process
}

// Print all background processes
void print_bg_processes() {
    bg_process_t *current = bg_head;
    int count = 0;

    while (current != NULL) {
        printf("%d: %s %d\n", current->pid, current->command, count+1);
        current = current->next;
        count++;
    }

    printf("Total Background jobs: %d\n", count);
}