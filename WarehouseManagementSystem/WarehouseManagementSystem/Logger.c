#define _CRT_SECURE_NO_WARNINGS
#include "Logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// dummy function need for the program to operate independently

char* getUser() { // Used on line 38
	return "SYS_ADM: ";
}

char* getItemName(int id) { // Used on line 98
	return "Stuff";
}

//  dummy stuff ends here

#define BUFFERSIZE 128
static FILE* logFile;
char textBuffer[BUFFERSIZE];

/*
Author; Alex
Input: char* containing the text that will be placed in file
Output: 0 if it fails. 1 if it succeeds
purpose: Write a string to the file
*/
int writeToFile(char* text) {
	// Check for buffer overflow
	if (strlen(text) >= BUFFERSIZE) {
		printf("ERROR: logger buffer overflow");
		return -1;
	}
	else if (logFile == NULL){
	printf("ERROR: Logger found no file open");
		return -1;
	}
	// get raw unix time
	time_t raw_time = time(NULL);
	// turn raw time into a structure of different values
	struct tm* local_time = localtime(&raw_time);
	//Turn into a string with format: YYYY-MM-DD HH:MM:SS
	strftime(textBuffer, sizeof(textBuffer), "<%Y-%m-%d %H:%M:%S> ", local_time);
	//append user
	strcat(textBuffer, getUser());
	//append the rest of the text to the buffer with timestamp
	strcat(textBuffer, text);
	//add \n
	strcat(textBuffer, "\n");
	// write buffer to file.Will return negative if it failed
	if (fprintf(logFile, textBuffer) < 0) {
		printf("ERROR: Logger failed to write to file.");
		return -1;
	}
	return 0;
}

/*
Author; Alex
Input: File path to log file
Output: 0 if it fails. 1 if it succeeds
purpose: on startup to open file and write start up message
*/
int logInit(char* filePath) {
	// open file in append only mode. Create new if one does not exist
	logFile = fopen(filePath, "a");
	//Check if file failed to open. iif file fails to open, exit
	if (logFile == NULL) {
		printf("Failed to open log file.");
		return -1;
	}
	//print startup message
	if (writeToFile("Program starting...") < 0) {
		printf("Failed to write to log file.");
		return -1;
	}
	return 0;
}

/*
Author; Alex
Input: Nothing
Output: Nothing
purpose: At end, print closing message, close file, clear buffer.
*/
void logCleanUp() {
	//Print closing message
	writeToFile("Program closing...");
	// close the file
	fclose(logFile);
	// Clear the buffer to leave no leftover information in memory
	memset(textBuffer, 0, sizeof(textBuffer));
	logFile = NULL;
}

/*
Author; Alex
Input: Transaction information
Output: 0 when the transaction is recorded successfully or -1 when writing fails.
purpose: Format and record a successful or rejected warehouse transaction.
*/
int logTransaction(int id, int amount, int info) {
	char transactionBuffer[BUFFERSIZE - 32] = { '0' };
	// add Id to text buffer
	sprintf(transactionBuffer + strlen(transactionBuffer), "%d", id);
	// Add product name
	strcat(transactionBuffer, "; ");
	strcat(transactionBuffer, getItemName(id));
	// Add amount name
	sprintf(transactionBuffer + strlen(transactionBuffer), "; %d", amount);
	// export if the number was 1
	if (info == -1) {
		strcat(transactionBuffer, "; Export");
	}
	else if (info == 1) {
		strcat(transactionBuffer, "; Import");
	}
	// The number was 0 or anything else
	else {
		strcat(transactionBuffer, "; Cancelled;");
	}
	// Pass string to write function. Also check if it failed to write
	if (writeToFile(transactionBuffer) < 0) {
		printf("ERROR: logger failed to log transaction");
		return -1;
	}
	return 0;
}
