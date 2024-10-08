#include <stdio.h>
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>
#include "ssi.h"

// Change the current directory
void change_directory(char *path) {
    if (path == NULL || strcmp(path, "~") == 0) {
        path = getenv("HOME");
    }

    if (chdir(path) != 0) {
        perror("cd failed");
    }
}