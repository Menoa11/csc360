#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sharedFunctions.h" // Include shared functions and definitions

// Retrieves directory entries by traversing the directory structure from the root
struct dir_entry_t** getDirectoryEntries(char* filename) {
    struct dir_entry_t* rootDir = calloc(1, sizeof(struct dir_entry_t)); // Allocate memory for the root directory structure
    rootDir->starting_block = superBlockInfo.root_dir_start_block; // Set starting block from superblock info
    rootDir->block_count = superBlockInfo.root_dir_block_count; // Set block count from superblock info

    struct dir_entry_t** rootEntries = exploreDirectory(rootDir); // Explore and retrieve root directory entries
    free(rootDir); // Free memory after use

    // If the directory path has only one token, return root entries
    if (numTokensDirectoryPath == 1) {
        return rootEntries;
    }

    struct dir_entry_t** currentEntries = rootEntries;
    int pathIndex = 0;

    // Traverse through the directory path tokens
    while (pathIndex < numTokensDirectoryPath) {
        int entryIndex = 0;
        int isFound = 0;

        // Look for the current directory in the list of entries
        while (entryIndex < numDirEntries) {
            if (strcmp((char*)directoryPath[pathIndex], (char*)currentEntries[entryIndex]->filename) == 0) {
                isFound = 1;
                break;
            }
            entryIndex++;
        }

        // If directory is found, check if it is a directory and update currentEntries
        if (isFound) {
            if ((currentEntries[entryIndex]->status & 0b0100) != 0b0100) {
                printf("\nError [6]: %s is not a directory\n", currentEntries[entryIndex]->filename);
                return NULL; // Not a directory
            }
            currentEntries = exploreDirectory(currentEntries[entryIndex]); // Explore next directory level
            if (pathIndex == numTokensDirectoryPath - 1) {
                return currentEntries; // Return when final directory reached
            }
        } else {
            printf("\nError [7]: Subdirectory %s does not exist\n", directoryPath[pathIndex]);
            return NULL; // Directory not found
        }

        pathIndex++; 
    }

    return NULL; // Return NULL if traversal fails
}

// Creates a file from the directory entry's contents
void createFileFromEntry(char* destinationFilename, struct dir_entry_t* entry) {
    FILE* destinationFile = fopen(destinationFilename, "wb");
    if (destinationFile == NULL) {
        printf("\nError [3]: Could not open file for writing\n");
        exit(EXIT_FAILURE);
    }

    int currentBlockAddress = entry->starting_block * blockSize; // Calculate starting block address
    int currentBlockCount = 0;
    int currentBytesRead = 0;

    // Loop through all blocks in the entry
    while (currentBlockCount < entry->block_count) {
        fseek(fp, currentBlockAddress, SEEK_SET); // Move file pointer to block address

        while (currentBytesRead < blockSize) {
            fread(buffer8, sizeof(char), 8, fp); // Read 8 bytes at a time
            fwrite(buffer8, sizeof(char), 8, destinationFile); // Write 8 bytes to the destination file
            currentBytesRead += 8;
        }

        currentBlockAddress = getNextBlock(currentBlockAddress / blockSize) * blockSize; // Get next block address
        currentBytesRead = 0; 
        currentBlockCount++;
    }

    fclose(destinationFile); // Close file after writing
}

// Finds a specific file in the directory structure based on the parsed path
struct dir_entry_t* findFileInDirectory() {
    char* targetFilename = (char*)calloc(31, sizeof(char)); // Allocate memory for the target filename
    if (targetFilename == NULL) {
        printf("\nError [8]: Memory allocation failed for targetFilename\n");
        return NULL;
    }
    strcpy(targetFilename, (char*)&directoryPath[numTokensDirectoryPath - 1]); // Copy last token as target filename

    struct dir_entry_t** directoryEntries = getDirectoryEntries(targetFilename);
    if (directoryEntries == NULL) {
        free(targetFilename);
        return NULL;
    }

    struct dir_entry_t* matchingEntry = NULL;
    for (int i = 0; i < numDirEntries; i++) {
        if (strcmp(targetFilename, (char*)directoryEntries[i]->filename) == 0) {
            matchingEntry = (struct dir_entry_t*)calloc(1, sizeof(struct dir_entry_t));
            if (matchingEntry == NULL) {
                printf("\nError [9]: Memory allocation failed for matchingEntry\n");
                break;
            }
            memcpy(matchingEntry, directoryEntries[i], sizeof(struct dir_entry_t)); // Copy matching entry data
            break;
        }
    }
    free(targetFilename); // Free memory for target filename
    return matchingEntry; // Return matching entry
}

// Main function: Handles command-line arguments and overall program flow
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("\nError [0]: Please include a .img file as the first argument\n");
        return EXIT_FAILURE;
    }
    if (argc < 3) {
        printf("\nError [1]: Please include the INPUT filename you want to copy from the .img file as the second argument\n");
        return EXIT_FAILURE;
    }
    if (argc < 4) {
        printf("\nError [2]: Please include the OUTPUT filename for the copied file from the .img file as the third argument\n");
        return EXIT_FAILURE;
    }

    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        printf("\nError [3]: Could not open file\n");
        return EXIT_FAILURE;
    }

    // Allocate buffers for reading and writing file data
    buffer1 = (char*)calloc(1, sizeof(char));
    buffer2 = (char*)calloc(2, sizeof(char));
    buffer4 = (char*)calloc(4, sizeof(char));
    buffer8 = (char*)calloc(8, sizeof(char));
    if (buffer1 == NULL || buffer2 == NULL || buffer4 == NULL || buffer8 == NULL) {
        printf("\nError [4]: Memory not allocated!\n");
        return EXIT_FAILURE;
    }

    loadSuperblock(); // Load filesystem metadata
    parseInputPath(argv[2]); // Parse the input path to locate file

    struct dir_entry_t* directoryEntry = findFileInDirectory();
    if (directoryEntry == NULL) {
        printf("\nFile not found.\n");
        return 0;
    }

    createFileFromEntry(argv[3], directoryEntry); // Create a file from the entry

    // Free allocated resources and close file
    free(directoryEntry);
    free(buffer1);
    free(buffer2);
    free(buffer4);
    free(buffer8);
    fclose(fp);

    return EXIT_SUCCESS;
}
