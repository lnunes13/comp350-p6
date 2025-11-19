// Student: Luciano Nunes, Jordan, Kevin
// Professor: Abdul Sattar
// Class: COMP350-001 Operating Systems
// Assignment: Project #6
// Description: This assignment is a group project to implement a simple file system that supports 6 operations

#include "file_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FILENAME_SIZE 32                                            // reserved bytes for filename
#define NUM_BLOCKS 100                                              // number of blocks in the disk
#define NUM_RESERVED_BLOCKS 10                                      // reserved blocks for the freemap
#define NUM_FILE_BLOCKS (NUM_BLOCKS - NUM_RESERVED_BLOCKS)
#define BLOCK_SIZE 512                                              // size of individual blocks in bytes (max file size is technically 480 bytes, since first 32 bytes include the filename)

int fileTable[NUM_FILE_BLOCKS];  // array of memory addresses of the files
char *disk;        // contiguous memory containing a number of bytes equal to (NUM_BLOCKS * MAX_FILE_SIZE)

const char BIT_MASKS[] = {1, 2, 4, 8, 16, 32, 64, 128}; // used to facilitate bitwise operations on filemap

// usage: stores the file into an available block
// on success returns the absolute block of the file's first byte in the disk;
// on failure returns -1
int createFile(char *file, int absoluteSize) {
    int reservedBlock = findFreeBlock();
    if (reservedBlock == -1)
        return reservedBlock;
    writeFileToDisk(reservedBlock, file, absoluteSize);
    writeToFileTable(reservedBlock);
    return reservedBlock;
}

// usage: prints file contents given a filename
void readFile(char* filename) {
    int i = searchFile(filename);
    if(i == -1) 
        printf("ERROR: file not found!");
    printf("File contents: \n"); 
}   

static void getFileContents(int absoluteBlock, char* filecontents) {
    memcpy(filecontents, disk + absoluteBlock*BLOCK_SIZE+FILENAME_SIZE, BLOCK_SIZE-FILENAME_SIZE);
    filecontents[BLOCK_SIZE-FILENAME_SIZE] = '\0';
}

// usage: searches for a file given a filename, returns the absolute block containing the file, returns -1 on file not found
static int searchFile(char *filename) {
    char searchName[FILENAME_SIZE+1];
    for(int i = 0; i < NUM_FILE_BLOCKS; i++) {
        if(fileTable[i] != -1) {
            getFileName(i, searchName);
            if(strcmp(filename, searchName) == 0) {
                return i;
            }
        }
    }
    return -1;
}

// usage: writes file into a block where 'file' is a pointer the file (including its name in the first 32 bytes)
static void writeFileToDisk(int absoluteBlock, char *file, int size) {
    for(int i = 0; i < size; i++)
        disk[absoluteBlock * NUM_BLOCKS + i] = file[i];
    bitmapReserveBlock(absoluteBlock);
}

// usage: writes a null character on all bytes of a file block
static void eraseBlock(int relativeBlock) {
    int absoluteBlock = relativeBlock + NUM_RESERVED_BLOCKS;
    for(int i = 0; i < BLOCK_SIZE; i++)
        disk[absoluteBlock + i] = '\0';
    bitmapFreeBlock(absoluteBlock);
}

// usage: extracts a filename as a C string from a given absolute block
static void getFileName(int absoluteBlock, char* filename) {
    char name[FILENAME_SIZE+1];
    int i = 0;
    while(i < FILENAME_SIZE+1 && disk[absoluteBlock * BLOCK_SIZE + i] != '\0' )
        name[i++] = disk[absoluteBlock * BLOCK_SIZE + i];
    name[i] = '\0';
    memcpy(filename, name, FILENAME_SIZE+1);
}

// usage: adds file to fileTable after it has been written to disk
static void writeToFileTable(int absoluteBlock) {
    int relativeBlock = absoluteBlock - NUM_RESERVED_BLOCKS;
    fileTable[relativeBlock] = absoluteBlock;
}

// usage: frees the block
static void removeFromFileTable(int absoluteBlock) {
    int relative_block = absoluteBlock - NUM_RESERVED_BLOCKS;
    fileTable[relative_block] = -1;
}

// usage: finds first free block and returns its absolute index
static int findFreeBlock() {
    int i = NUM_RESERVED_BLOCKS;
    while(i < NUM_BLOCKS && isBlockReserved(i))
        i++;
    if (i >= NUM_BLOCKS)
        return -1;
    return i;
}

// usage: reserves block in the filemap
static void bitmapReserveBlock(int absoluteBlock) {
    int pos = absoluteBlock / 8;
    int bit = absoluteBlock % 8;
    disk[pos] = disk[pos] | BIT_MASKS[bit];
}

// usage: frees block in the filemap
static void bitmapFreeBlock(int absoluteBlock) {
    int pos = absoluteBlock / 8;
    int bit = absoluteBlock % 8;
    disk[pos] = disk[pos] & ~BIT_MASKS[bit];
}

// usage: checks if block is already reserved
static int isBlockReserved(int absoluteBlock) {
    int pos = absoluteBlock / 8;
    int bit = absoluteBlock % 8;
    int a = disk[pos];
    int b = BIT_MASKS[bit];
    int c = disk[pos] & BIT_MASKS[bit];
    return c;
}

// usage: sets all file table entries to null
static void clearFileTable() {
    for(int i = 0; i < NUM_FILE_BLOCKS; i++)
        fileTable[i] = -1;
}

// usage: set all reserved bytes to 0 and all file bytes to null characters
static void clearDisk() {
    memset(disk, '\0', NUM_BLOCKS * BLOCK_SIZE);
}

// usage: called once at the beginning of the program to allocate system memory to simulate disk
void allocateFileSystem(){
    disk = (char*)malloc(NUM_BLOCKS * BLOCK_SIZE * sizeof(char));
    memset(fileTable, -1, NUM_FILE_BLOCKS * sizeof(int));
}

// clears entireDisk and filetable
void formatDisk() {
    clearDisk();
    clearFileTable();
}

// usage: called once at the end of the program to free system memory
void freeFileSystem() {
    free(disk);
}

// usage: lists all contents in the directory
void printDirectory() {
    char filename[FILENAME_SIZE+1];
    char filecontents[BLOCK_SIZE-FILENAME_SIZE+1];
    printf("%-8s %-32s %-16s\n", "block", "filename", "preview");
    printf("%-8s %-32s %-16s\n", "-----", "--------", "-------");
    for(int i = 0; i < NUM_FILE_BLOCKS; i++) {
        if(fileTable[i] != -1) {
            getFileName(fileTable[i], filename);
            getFileContents(fileTable[i], filecontents);
            printf("%-8d %-32s %-16s\n", i, filename, filecontents); 
        }
    }
}