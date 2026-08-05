#pragma once
/*
Author; Alex
Input: char* containing the text that will be placed in file
Output: 0 if it fails. 1 if it succeeds
purpose: Write a string to the file
*/
int writeToFile(char* text);

/*
Author; Alex
Input: File path to log file
Output: 0 if it fails. 1 if it succeeds
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
int logTransaction(int id, int amount, int IO, int approved);

/*test cases

The Logger module shall append a new entry to the transaction log file each time a transaction is processed.
The Logger module shall record a timestamp for every transaction entry.
test case: did the program write to file as intended
1 test to make the function return a string and check if it matches?



The Logger module shall receive the log file path from Main and shall not hardcode any file paths internally.
The Logger module shall open and close the log file for each transaction and shall not leave any file handles open between calls.
The Logger module shall write a session start entry to the log file when the program starts.
test case: did it open file correctly and leave start up message
1 test since the only thing that can change is file path and it will create new if one inst there.

The Logger module shall record a cancelled status for any transaction that could not be completed.
The Logger module shall indicate whether each transaction was incoming or outgoing stock.
The Logger module shall receive transaction details from the Orders module and write them to the log file.
The Logger module shall record the product ID, product name, quantity, and timestamp for each transaction.
test case: did it create the order string as intended
6 tests: if it is outgoing. incoming. id does not match any item. quaintly is not possible, working as intended. if it was cancelled

The Logger module shall call the Utilities module to validate all inputs before writing to the log file.
what?
*/
