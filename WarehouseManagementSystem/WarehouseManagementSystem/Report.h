#pragma once
#include "inventory.h"

/*
Author; Alex
Input: char* with a pointer to the buffer it will add the name of the report to
Output: nothing
purpose: Append the name of the report to the already existing buffer
*/
void genReportName(char* buffer);
/*
Author; Alex
Input: a pointer to the first node of the inventory list. char* with the path to the folder the file is in
Output: -1 if it fails. 0 if it succeeds
purpose: Generate the report using the filepath and link to first node in list. list is a .txt
*/
int report_generate_text(Product** head, const char* filePath);
/*
Author; Alex
Input: a pointer to the first node of the inventory list. char* with the path to the folder the file is in
Output: -1 if it fails. 0 if it succeeds
purpose: Generate the report using the filepath and link to first node in list. list is a .csv
*/
int report_generate_csv(Product** head, const char* filePath);