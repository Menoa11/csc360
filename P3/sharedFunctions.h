#ifndef SHARED_FUNCTIONS_H
#define SHARED_FUNCTIONS_H

#include <stdio.h>
#include <stdint.h>

// Definition of the superblock structure, containing metadata about the filesystem
struct __attribute__((__packed__)) superblock_t {
    __uint8_t fs_id[8];               // Filesystem identifier
    __uint16_t block_size;            // Size of each block in bytes
    __uint32_t file_system_block_count; // Total number of blocks in the filesystem
    __uint32_t fat_start_block;       // Starting block of the File Allocation Table (FAT)
    __uint32_t fat_block_count;       // Number of blocks used by FAT
    __uint32_t root_dir_start_block;  // Starting block of the root directory
    __uint32_t root_dir_block_count;  // Number of blocks in the root directory
};

// Structure to store timestamp information for directory entries
struct __attribute__((__packed__)) dir_entry_timedate_t {
    __uint16_t year;   // Year of modification or creation
    __uint8_t month;   // Month of modification or creation
    __uint8_t day;     // Day of modification or creation
    __uint8_t hour;    // Hour of modification or creation
    __uint8_t minute;  // Minute of modification or creation
    __uint8_t second;  // Second of modification or creation
};

// Structure representing a directory entry in the filesystem
struct __attribute__((__packed__)) dir_entry_t {
    __uint8_t status;                   // Status of the entry (e.g., file, directory)
    __uint32_t starting_block;          // Block where the file/directory starts
    __uint32_t block_count;             // Number of blocks occupied
    __uint32_t size;                    // Size of the file in bytes
    struct dir_entry_timedate_t create_time; // Creation timestamp
    struct dir_entry_timedate_t modify_time; // Last modification timestamp
    __uint8_t filename[31];             // Filename (up to 31 characters)
    __uint8_t unused[6];                // Padding to align the structure
};

// Global variables defined elsewhere, used for file operations and path parsing
extern struct superblock_t superBlockInfo;
extern char *buffer1, *buffer2, *buffer4, *buffer8;
extern FILE* fp;
extern __uint8_t directoryPath[10][31]; // Stores directory path tokens
extern int numTokensDirectoryPath;      // Number of tokens in the directory path
extern int numDirEntries;               // Number of entries in the current directory
extern int blockSize;                   // Size of a block in bytes

// Function prototypes for filesystem operations
void loadSuperblock();                          // Load superblock information from the disk
void parseInputPath(char* src);                 // Parse the input path into directory tokens
struct dir_entry_t** fetchDirectoryEntries(int blockNum); // Fetch directory entries from a specific block
struct dir_entry_t** exploreDirectory(struct dir_entry_t* searchDirectoryEntry); // Recursively explore a directory
__uint32_t getNextBlock(__uint32_t currentBlock); // Retrieve the next block in the chain from the FAT

#endif // SHARED_FUNCTIONS_H