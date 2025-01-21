#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Implemented this before the header file so its independant of it

#define BLOCK_SIZE 512  // Define the size of a block as 512 bytes

// Structure to hold superblock information
struct SuperBlock {
    short blockSize;        // Size of each block
    int blockCount;         // Total number of blocks in the file system
    int fatStart;           // Starting block of the FAT
    int fatBlockCount;      // Number of blocks occupied by the FAT
    int rootStart;          // Starting block of the root directory
    int rootBlockCount;     // Number of blocks in the root directory
};

// Global variables for superblock information and file handling
struct SuperBlock superBlock;
char *buf1, *buf2, *buf4;  // Buffers for reading file data
FILE* file;                // File pointer for the disk image

// Counters for different block types in the FAT
int freeBlocks = 0, reservedBlocks = 0, allocatedBlocks = 0;

// Function to read and parse the superblock information
void readSuperBlock() {
    // Skip the first 8 bytes (identifier section)
    fseek(file, 8, SEEK_CUR);

    // Read the block size (2 bytes)
    fread(buf2, 1, 2, file);
    superBlock.blockSize = (buf2[0] << 8) | buf2[1];

    // Read the total number of blocks (4 bytes)
    fread(buf4, 1, 4, file);
    superBlock.blockCount = (buf4[0] << 24) | (buf4[1] << 16) | (buf4[2] << 8) | buf4[3];

    // Read the starting block of the FAT (4 bytes)
    fread(buf4, 1, 4, file);
    superBlock.fatStart = (buf4[0] << 24) | (buf4[1] << 16) | (buf4[2] << 8) | buf4[3];

    // Read the number of FAT blocks (4 bytes)
    fread(buf4, 1, 4, file);
    superBlock.fatBlockCount = (buf4[0] << 24) | (buf4[1] << 16) | (buf4[2] << 8) | buf4[3];

    // Read the starting block of the root directory (4 bytes)
    fread(buf4, 1, 4, file);
    superBlock.rootStart = (buf4[0] << 24) | (buf4[1] << 16) | (buf4[2] << 8) | buf4[3];

    // Read the number of root directory blocks (4 bytes)
    fread(buf4, 1, 4, file);
    superBlock.rootBlockCount = (buf4[0] << 24) | (buf4[1] << 16) | (buf4[2] << 8) | buf4[3];
}

// Function to analyze FAT blocks and count free, reserved, and allocated blocks
void analyzeFATBlocks() {
    int fatValue;
    int fatStartAddr = superBlock.fatStart * BLOCK_SIZE;
    int fatEndAddr = (superBlock.fatStart + superBlock.fatBlockCount) * BLOCK_SIZE;

    // Loop through FAT entries and categorize blocks
    while (fatStartAddr < fatEndAddr) {
        fseek(file, fatStartAddr, SEEK_SET);
        fread(buf4, 1, 4, file);
        fatValue = (buf4[0] << 24) | (buf4[1] << 16) | (buf4[2] << 8) | buf4[3];

        if (fatValue == 0) {
            freeBlocks++;  // Unused block
        } else if (fatValue == 1) {
            reservedBlocks++;  // Reserved block
        } else {
            allocatedBlocks++;  // Allocated block
        }
        fatStartAddr += 4;  // Move to the next FAT entry (4 bytes)
    }
}

// Function to display superblock and FAT information
void displaySuperBlock() {
    printf("Super block information:\n");
    printf("Block size: %d\n", superBlock.blockSize);
    printf("Block count: %d\n", superBlock.blockCount);
    printf("FAT starts: %d\n", superBlock.fatStart);
    printf("FAT blocks: %d\n", superBlock.fatBlockCount);
    printf("Root directory start: %d\n", superBlock.rootStart);
    printf("Root directory blocks: %d\n", superBlock.rootBlockCount);
    printf("\nFAT information:\n");
    printf("Free Blocks: %d\n", freeBlocks);
    printf("Reserved Blocks: %d\n", reservedBlocks);
    printf("Allocated Blocks: %d\n", allocatedBlocks);
}

int main(int argc, char *argv[]) {
    // Ensure a file path is provided as an argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <image file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open the disk image file
    file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for buffers
    buf1 = calloc(1, 1);
    buf2 = calloc(2, 1);
    buf4 = calloc(4, 1);
    if (!buf1 || !buf2 || !buf4) {
        perror("Memory allocation failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Read and process the superblock, analyze FAT blocks, and display results
    readSuperBlock();
    analyzeFATBlocks();
    displaySuperBlock();

    // Free allocated memory and close the file
    free(buf1);
    free(buf2);
    free(buf4);
    fclose(file);

    return 0;
}