#define _CRT_SECURE_NO_WARNINGS
#include "Report.h"
#include "inventory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

// Generate the name of the file into the string given
int genReportName(char* buffer) {
	// Generate a buffer to store time
	char timeBuffer[32] = { '0' };
	// Add work report
	strcat(buffer, "Report ");
	// get raw unix time
	time_t raw_time = time(NULL);
	// turn raw time into a structure of different values
	struct tm* local_time = localtime(&raw_time);
	//Turn into a string with format: YYYY-MM-DD HH:MM:SS
	strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H-%M-%S", local_time);
	// Add time stamp to file name
	strcat(buffer, timeBuffer);
	Sleep(1000); // makes it impossible for two reports with the same name
	return 0;
}
int report_generate_text(Product** head, const char* filePath) {
	// Create a buffer for the name
	char fileName[64] = { '0' };
	// Add the predefined file path to the buffer
	sprintf(fileName, "%s", filePath);
	genReportName(fileName);
	// Add file type
	strcat(fileName, ".txt");
	// open the file
	FILE* reportFile = fopen(fileName, "w");
	Product* currentNode = *head;
	while (currentNode != NULL) {
		fprintf(reportFile, "ID: %d	Name: %s	Amount: %d	Location: %s\n", currentNode->id, currentNode->name, currentNode->quantity, currentNode->location);
		currentNode = currentNode->next;
	}
	return 0;
}

int report_generate_csv(Product** head, const char* filePath) {
	// Create a buffer for the name
	char fileName[64] = { '0' };
	// Add the predefined file path to the buffer
	sprintf(fileName, "%s", filePath);
	genReportName(fileName);
	// Add file type
	strcat(fileName, ".csv");
	// open the file
	FILE* reportFile = fopen(fileName, "w");
	Product* currentNode = *head;
	while (currentNode != NULL) {
		fprintf(reportFile, "%d,%s,%d,%s\n", currentNode->id, currentNode->name, currentNode->quantity, currentNode->location);
		currentNode = currentNode->next;
	}
	return 0;
}

