#ifndef SSI_H
#define SSI_H

// Constants
#define MAX_CMD_LEN 256

// Structure for a background process linked list (Same node keys as tutorial suggests)
typedef struct bg_process {
    int pid;                      // Process ID
    char command[MAX_CMD_LEN];    // Command executed
    struct bg_process *next;      // Pointer to the next process
} bg_process_t;

extern bg_process_t *bg_head;     // Needed to display node command when terminted
extern pid_t foreground_pid;      // Needed for Ctrl+c command termination
extern int shell_printed;

// Function declarations
void execute_foreground(char *input);
void execute_background(char *input);
void change_directory(char *path);
void add_bg_process(int pid, char *command);
void remove_bg_process(int pid);
void print_bg_processes();
void handle_sigchld(int sig);
void sigint_handler(int sig);
void check_background_processes();
void kill_process(char *input);

#endif // SSI_H