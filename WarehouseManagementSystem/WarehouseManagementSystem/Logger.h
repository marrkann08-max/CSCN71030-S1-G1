#pragma once
#include <stdio.h>

/*
Author; Alex
Input: char* containing the text that will be placed in file and the file pointer because units test break without it
Output: 0 if it fails. 1 if it succeeds
purpose: Write a string to the file
*/
int writeToFile(char* text);

/*
Author; Alex
Input: File path to log file
Output: -1 if it fails. 0 if it succeeds
purpose: on startup to open file and write start up message
*/
int logInit(char* filePath);

/*
Author; Alex
Input: Nothing
Output: Nothing
purpose: At end, print closing message, close file, clear buffer.
*/
void logCleanUp();

/*
Author; Alex
Input: Transaction information
Output: -1 if it failed to write to file. 0 if it succeeded.
purpose: At end, print closing message, close file, clear buffer.
*/
int logTransaction(int id, int amount, int info);
