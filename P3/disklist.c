#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "sharedFunctions.h" // Include shared functions for handling the filesystem

// Displays directory entries with their details: file type, size, name, and modification date/time
void showDirectoryEntries(struct dir_entry_t** directoryEntries) {
    char date[11];  // Buffer for storing date string (YYYY/MM/DD)
    char time[9];   // Buffer for storing time string (HH:MM:SS)
    int numEntries = numDirEntries;

    for (int ctr = 0; ctr < numEntries; ctr++) {
        if (directoryEntries[ctr]->status == 0) continue; // Skip if entry is inactive

        // Determine file type: F (File), D (Directory), N (None)
        char fileType = (directoryEntries[ctr]->status & 0b0010) ? 'F' : 
                        (directoryEntries[ctr]->status & 0b0100) ? 'D' : 'N';

        // Format modification date and time
        snprintf(date, sizeof(date), "%04u/%02u/%02u", 
                 directoryEntries[ctr]->modify_time.year, 
                 directoryEntries[ctr]->modify_time.month, 
                 directoryEntries[ctr]->modify_time.day);

        snprintf(time, sizeof(time), "%02u:%02u:%02u", 
                 directoryEntries[ctr]->modify_time.hour, 
                 directoryEntries[ctr]->modify_time.minute, 
                 directoryEntries[ctr]->modify_time.second);

        // Print file type, size, name, date, and time
        printf("%c %10u %30s %10s %8s\n", fileType, 
               directoryEntries[ctr]->size, 
               directoryEntries[ctr]->filename, 
               date, time);
    }
}

// Resolves and retrieves file or directory entries based on the input path
struct dir_entry_t** resolveFileEntries() {
    // Initialize root directory based on superblock info
    struct dir_entry_t* rootDirectory = (struct dir_entry_t*) calloc(1, sizeof(struct dir_entry_t));
    rootDirectory->starting_block = superBlockInfo.root_dir_start_block;
    rootDirectory->block_count = superBlockInfo.root_dir_block_count;
    
    struct dir_entry_t** directoryEntries = exploreDirectory(rootDirectory); // Get entries from root
    free(rootDirectory); // Free allocated root directory structure

    if (numTokensDirectoryPath < 1) {
        return directoryEntries; // Return if no specific path is provided
    }

    struct dir_entry_t** currentDirectoryEntries = directoryEntries;

    // Traverse through each token in the directory path
    for (int dirPathCtr = 0; dirPathCtr < numTokensDirectoryPath; dirPathCtr++) {
        int match = 0;
        for (int dirEntriesCtr = 0; dirEntriesCtr < numDirEntries; dirEntriesCtr++) {
            // Compare directory path token with current directory entry's filename
            if (strcmp((char*)directoryPath[dirPathCtr], (char*)currentDirectoryEntries[dirEntriesCtr]->filename) == 0) {
                // Check if the entry is a directory
                if ((currentDirectoryEntries[dirEntriesCtr]->status & 0b0100) != 0b0100) {
                    printf("\nError [6]: %s is not a directory\n", currentDirectoryEntries[dirEntriesCtr]->filename);
                    return NULL; // Return NULL if not a directory
                }
                // Move deeper into the directory structure
                currentDirectoryEntries = exploreDirectory(currentDirectoryEntries[dirEntriesCtr]);
                match = 1;
                break;
            }
        }
        if (!match) {
            printf("\nError [7]: Subdirectory %s does not exist\n", directoryPath[dirPathCtr]);
            return NULL; // Return NULL if subdirectory does not exist
        }
    }

    return currentDirectoryEntries; // Return resolved directory entries
}

// Main function: Initializes and manages program execution
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("\nError [0]: Please include a .img file as the first argument\n");
        return EXIT_FAILURE; // Exit if no .img file is provided
    }

    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        printf("\nError [3]: Could not open file\n");
        return EXIT_FAILURE; // Exit if the .img file can't be opened
    }

    // Allocate memory for buffers used in file handling
    buffer1 = (char*) calloc(1, sizeof(char));
    buffer2 = (char*) calloc(2, sizeof(char));
    buffer4 = (char*) calloc(4, sizeof(char));
    buffer8 = (char*) calloc(8, sizeof(char));
    if (buffer1 == NULL || buffer2 == NULL || buffer4 == NULL || buffer8 == NULL) {
        printf("\nError [4]: Memory not allocated!\n");
        fclose(fp);
        return EXIT_FAILURE; // Exit if memory allocation fails
    }

    loadSuperblock(); // Load filesystem metadata

    if (argc > 2) {
        parseInputPath(argv[2]); // Parse provided input path if specified
    }

    // Resolve directory entries and display them
    struct dir_entry_t** directoryEntries = resolveFileEntries();
    if (directoryEntries != NULL) {
        showDirectoryEntries(directoryEntries);
        // Free individual directory entries
        for (int i = 0; i < numDirEntries; i++) {
            free(directoryEntries[i]);
        }
        free(directoryEntries); // Free directory entries array
    }

    // Free allocated buffers and close file
    free(buffer1);
    free(buffer2);
    free(buffer4);
    free(buffer8);
    fclose(fp);

    return EXIT_SUCCESS;
}