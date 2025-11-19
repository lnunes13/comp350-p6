// Student: Luciano Nunes, Jordan de Andrade, Peter House
// Professor: Abdul Sattar
// Class: COMP350-001 Operating Systems
// Assignment: Project #6
// Description: This assignment is a group project to implement a simple file system that supports 6 operations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_system.c"

#define INPUT_BUFFER_SIZE 1024

void getUserInput(char *buffer, int size);
void clearInputBuffer();
void formatToFile(char* inputBuffer, char* filebuffer);
void printFileTest(char *fileBuffer);

int main() {
    char inputBuffer[INPUT_BUFFER_SIZE];
    char tokenBuffer[INPUT_BUFFER_SIZE];
    char *token;

    allocateFileSystem();

    do {
        printf("filesystem> ");
        fgets(inputBuffer, INPUT_BUFFER_SIZE, stdin);
        memcpy(tokenBuffer, inputBuffer, INPUT_BUFFER_SIZE);
        token = strlwr(strtok(tokenBuffer, " \n"));

        if(strcmp(token, "format") == 0) {
            printf("formatting disk...\n");
            formatDisk();
            printf("disk formatted\n");
        } else if(strcmp(token, "create") == 0) {
            char fileBuffer[BLOCK_SIZE];
            formatToFile(inputBuffer, fileBuffer);
            createFile(fileBuffer, BLOCK_SIZE);
        } else if(strcmp(token, "read") == 0) {
            token = strtok(NULL, " \n");
            readFile(token);
        } else if(strcmp(token, "write") == 0) {
            
        } else if(strcmp(token, "delete") == 0) {
            
        } else if(strcmp(token, "dir") == 0) {
            printDirectory();
        } 
    } while(strcmp(token, "exit") != 0);

    freeFileSystem();
}

void formatToFile(char* inputBuffer, char* fileBuffer) {
    char filename[FILENAME_SIZE];
    char filedata[BLOCK_SIZE - FILENAME_SIZE];
    
    // getfilename
    char tokenBuffer[INPUT_BUFFER_SIZE];
    memcpy(tokenBuffer, inputBuffer, INPUT_BUFFER_SIZE);
    char *token = strtok(tokenBuffer, " \n");
    token = strtok(NULL, " \n");
    strcpy(filename, token);

    // getfiledata
    token = strtok(NULL, " \n");
    char *datastart = strstr(inputBuffer, token);

    // copy values into filebuffer
    memcpy(fileBuffer, filename, FILENAME_SIZE);
    memcpy(filedata, datastart, BLOCK_SIZE - FILENAME_SIZE);
    memcpy(fileBuffer + FILENAME_SIZE, filedata, BLOCK_SIZE - FILENAME_SIZE);
}

void printFileTest(char *fileBuffer) {
    for(int i = 0; i < BLOCK_SIZE; i++) {
        printf("%c", fileBuffer[i]);
    }
}