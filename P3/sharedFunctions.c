#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sharedFunctions.h"  // Custom shared functions header file

// Constants and global variables
int blockSize = 512;  // Block size used in the file system
struct superblock_t superBlockInfo;  
char *buffer1, *buffer2, *buffer4, *buffer8;  
FILE* fp;  
__uint8_t directoryPath[10][31]; 
int numTokensDirectoryPath = 0;  // Number of tokens in the directory path
int numDirEntries = 0;  // Number of directory entries

// Function declarations
void loadSuperblock();
void parseInputPath(char* src);
struct dir_entry_t** fetchDirectoryEntries(int blockNum);
struct dir_entry_t** exploreDirectory(struct dir_entry_t* searchDirectoryEntry);
__uint32_t getNextBlock(__uint32_t currentBlock);

// Load superblock information from the file system image
void loadSuperblock() {
    fread(buffer8, sizeof(char), 8, fp);  // Read 8 bytes for file system ID
    memcpy(superBlockInfo.fs_id, buffer8, sizeof(char) * 8);  // Copy to superblock struct

    fread(buffer2, sizeof(char), 2, fp);  // Read 2 bytes for block size
    superBlockInfo.block_size = buffer2[0] << 8 | buffer2[1];  // Convert to integer

    fread(buffer4, sizeof(char), 4, fp);  // Read 4 bytes for block count
    superBlockInfo.file_system_block_count = (__uint64_t) buffer4[0] << 32 | buffer4[1] << 16 | buffer4[2] << 8 | buffer4[3];

    fread(buffer4, sizeof(char), 4, fp);  // FAT start block
    superBlockInfo.fat_start_block = (__uint64_t) buffer4[0] << 32 | buffer4[1] << 16 | buffer4[2] << 8 | buffer4[3];

    fread(buffer4, sizeof(char), 4, fp);  // FAT block count
    superBlockInfo.fat_block_count = (__uint64_t) buffer4[0] << 32 | buffer4[1] << 16 | buffer4[2] << 8 | buffer4[3];

    fread(buffer4, sizeof(char), 4, fp);  // Root directory start block
    superBlockInfo.root_dir_start_block = (__uint64_t) buffer4[0] << 32 | buffer4[1] << 16 | buffer4[2] << 8 | buffer4[3];

    fread(buffer4, sizeof(char), 4, fp);  // Root directory block count
    superBlockInfo.root_dir_block_count = (__uint64_t) buffer4[0] << 32 | buffer4[1] << 16 | buffer4[2] << 8 | buffer4[3];
}

// Parse a given input path into individual directory tokens
void parseInputPath(char* src) {
    __uint8_t* token = (__uint8_t *) calloc(31, sizeof(char));  // Allocate memory for token
    token = (__uint8_t *) strtok(src, "/");  // Tokenize path by "/"
    
    while (token != NULL) {
        strcpy((char *) &directoryPath[numTokensDirectoryPath], (char *) token);  // Store token
        numTokensDirectoryPath++;  // Increment token count
        token = (__uint8_t *) strtok(NULL, "/");  // Get next token
    }
}

// Explore a directory and return all its entries
struct dir_entry_t** exploreDirectory(struct dir_entry_t* searchDirectoryEntry) {
    numDirEntries = searchDirectoryEntry->block_count * 8;  // Calculate total entries
    struct dir_entry_t** directoryEntries = (struct dir_entry_t **) calloc(numDirEntries, sizeof(struct dir_entry_t));
    __uint32_t currentBlock = searchDirectoryEntry->starting_block;  // Start block of directory
    
    for (int blockCtr = 0, offset = 0; blockCtr < searchDirectoryEntry->block_count; blockCtr++, offset += 8) {
        struct dir_entry_t** currentDirectoryEntries = fetchDirectoryEntries(currentBlock);
        memcpy(&directoryEntries[offset], currentDirectoryEntries, sizeof(struct dir_entry_t) * 8);  // Copy entries
        currentBlock = getNextBlock(currentBlock);  // Get next block in the chain
        free(currentDirectoryEntries);  // Free memory
    }

    return directoryEntries;
}

// Fetch directory entries from a specific block number
struct dir_entry_t** fetchDirectoryEntries(int blockNum) {
    int dirStartPoint = blockNum * blockSize;  // Start byte of the block
    int dirEndPoint = (blockNum + 1) * blockSize;  // End byte of the block

    struct dir_entry_t** directoryEntries = calloc(8, sizeof(struct dir_entry_t));  // Allocate memory for 8 entries
    struct dir_entry_t* directoryEntry;
    int arrIndex = 0;  // Index to store entries

    while (dirStartPoint < dirEndPoint) {
        directoryEntry = (struct dir_entry_t *) calloc(1, sizeof(struct dir_entry_t));  // Allocate a new entry

        fseek(fp, dirStartPoint, SEEK_SET);  // Seek to the current position
        
        fread(buffer1, sizeof(char), 1, fp);  // Read status
        directoryEntry->status = buffer1[0];
        
        fread(buffer4, sizeof(char), 4, fp);  // Starting block of the file
        directoryEntry->starting_block = (__uint64_t) buffer4[0] << 32 | buffer4[1] << 16 | buffer4[2] << 8 | buffer4[3];
        
        fread(buffer4, sizeof(char), 4, fp);  // Number of blocks
        directoryEntry->block_count = (__uint64_t) buffer4[0] << 32 | buffer4[1] << 16 | buffer4[2] << 8 | buffer4[3];

        fread(buffer4, sizeof(char), 4, fp);  // File size
        directoryEntry->size = (__uint64_t) buffer4[0] << 32 | buffer4[1] << 16 | buffer4[2] << 8 | (buffer4[3] & 0b11111111);

        // File creation time
        fread(buffer8, sizeof(char), 7, fp);
        directoryEntry->create_time.year = buffer8[0] << 8 | (buffer8[1] & 0xFF);
        directoryEntry->create_time.month = buffer8[2];
        directoryEntry->create_time.day = buffer8[3];
        directoryEntry->create_time.hour = buffer8[4];
        directoryEntry->create_time.minute = buffer8[5];
        directoryEntry->create_time.second = buffer8[6];

        // File modification time
        fread(buffer8, sizeof(char), 7, fp);
        directoryEntry->modify_time.year = buffer8[0] << 8 | (buffer8[1] & 0xFF);
        directoryEntry->modify_time.month = buffer8[2];
        directoryEntry->modify_time.day = buffer8[3];
        directoryEntry->modify_time.hour = buffer8[4];
        directoryEntry->modify_time.minute = buffer8[5];
        directoryEntry->modify_time.second = buffer8[6];

        fread(directoryEntry->filename, sizeof(char), 31, fp);  // Read filename
        fread(directoryEntry->unused, sizeof(char), 6, fp);  // Read unused bytes
    
        dirStartPoint += 64;  // Move to next entry
        directoryEntries[arrIndex++] = directoryEntry;
    }
    
    return directoryEntries;
}

// Get the next block in the FAT chain
__uint32_t getNextBlock(__uint32_t currentBlock) {
    __uint32_t dirEntryFATBlockNum = (currentBlock / 128);  // Block in FAT table
    __uint32_t dirEntryFATEntryNum = (currentBlock % 128);  // Entry index within the block
    __uint32_t FATEntrySize = 4;  // Each FAT entry is 4 bytes
    __uint32_t FATEntryPoint = (((superBlockInfo.fat_start_block + dirEntryFATBlockNum) * blockSize) + (dirEntryFATEntryNum * FATEntrySize));
    __uint32_t FATEntryValue;

    fseek(fp, FATEntryPoint, SEEK_SET);  // Seek to FAT entry position
    fread(buffer4, sizeof(char), 4, fp);  // Read FAT entry
    FATEntryValue = (__uint64_t) buffer4[0] << 32 | buffer4[1] << 16 | buffer4[2] << 8 | buffer4[3];
    
    return FATEntryValue;  // Return next block
}