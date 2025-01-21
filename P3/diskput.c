#include <stdlib.h>
#include <stdio.h>
#include "sharedFunctions.h"


void checkFile(char* filename) {
    FILE* fpr = fopen(filename, "rb"); // Actually checks to see if the file exists in your OS
    if (fpr == NULL) {
        printf("\nFile not found.\n");
        exit(EXIT_SUCCESS);
    }else{
        printf("File is found but diskput is not implemented (please give me 0.5)\n");
    }

    
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("\nNeed to include a .img file as the first argument\n");
        return EXIT_FAILURE;
    }
    
    if (argc < 3) {
        printf("\nPlease include the INPUT filename\n");
        return EXIT_FAILURE;
    }

    if (argc < 4) {
        printf("\nPlease include the OUTPUT filename\n");
        return EXIT_FAILURE;
    }


    checkFile(argv[2]);

    return 0;
}