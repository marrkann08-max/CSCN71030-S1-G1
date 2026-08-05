#define _CRT_SECURE_NO_WARNINGS
#include "Logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Dummy.h"



FILE* logFile;
char textBuffer[256];
char* user;

/*
Author; Alex
Input: char* containing the text that will be placed in file
Output: 0 if it fails. 1 if it succeeds
purpose: Write a string to the file
*/
int writeToFile(char* text) {
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
	// write buffer to file
	fprintf(logFile, textBuffer);
	return 1;
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
		return 0;
	}
	//print startup message
	writeToFile("Program starting...");
	return 1;
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
	// overwrite then clear the buffer to leave no leftover information in memory
}

/*
Author; Alex
Input: Transaction information
Output: 1 when the transaction is recorded successfully or -1 when formatting or writing fails.
purpose: Format and record a successful or rejected warehouse transaction.
*/
int logTransaction(int id, int amount, int IO, int approved) {
	char transactionBuffer[128] = { '0' };
	// add Id to text buffer
	sprintf(transactionBuffer + strlen(transactionBuffer), "%d", id);
	// Add product name
	strcat(transactionBuffer, "; ");
	strcat(transactionBuffer, getItemName(id));
	// Add amount name
	sprintf(transactionBuffer + strlen(transactionBuffer), "; %d", amount);
	// Iif it was an export
	if (approved == -1) {
		strcat(transactionBuffer, "; Export");
	}
	else {
		strcat(transactionBuffer, "; Import");
	}
	// Pass string to write function
	// iif it was approved
	if (approved == 1) {
		strcat(transactionBuffer, "; Approved;");
	}
	else {
		strcat(transactionBuffer, "; Cancelled;");
	}
	// Pass string to write function
	writeToFile(transactionBuffer);
	return 1;
}