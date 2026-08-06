#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"search.h"
static SearchHistoryEntry history[SEARCH_HISTORY_CAPACITY];
static int historyCount = 0;
/*Function: search_by_id
Purpose:Search the Inventory linked list for a Product with the specified ID
Inputs:
head - Pointer to the first Product in the Inventory.
productID - Product ID to search for
Output:Returns a pointer to the matching Product if found.
Returns:Product* - Pointer to the matching Product, or NULL if not found.
Author:Inderpreet Kaur Hundal*/
Product* search_by_id(Product* head, unsigned int productID) {
    Product* current = head; // Start searching from the first Product in the Inventory.

    if (head == NULL || productID == 0) // validate input parameters
    {
	return NULL;
}

    while (current != NULL) // Traverse the linked list until the Product is found or the end is reached.
    {
        if (current->id == productID)
        {
            return current;
        }

        current = current->next;
    }
	return NULL; // product was not found
}
/*Function: containsIgnoreCase
Purpose:Determine whether a Product name contains the specified search keyword without considering letter case.
Inputs:
text - Product name to search.
pattern - Search keyword.
Output:None.
Returns:int - Returns 1 if the keyword is found, otherwise returns 0.
Author:Inderpreet Kaur Hundal*/
static int containsIgnoreCase(const char* text, const char* pattern) {
    while (*text) // Check each possible starting position in the Product name.
    {
        const char* t = text;
        const char* p = pattern;
        // Compare both strings without considering letter case.
        while (*t && *p && tolower((unsigned char)*t) == tolower((unsigned char)*p)) {
            t++;
            p++;
        }
        // Entire search keyword matched.
        if (*p == '\0') {
            return 1;
        }
        text++;
    }
    return 0;
}
/*Function: search_by_name
Purpose:Search the Inventory for Products whose names contain the specified keyword without considering letter case.
Inputs:
head - Pointer to the first Product in the Inventory.
keyword - Search keyword.
result - Pointer to a SearchResult structure to store matching Products.
Output:Stores pointers to all matching Products in the SearchResult structure.
Returns:int - Returns 0 on success or -1 if the input is invalid or memory allocation fails.
Author:Inderpreet Kaur Hundal*/
int search_by_name(Product* head, const char* keyword, SearchResult* result) {
    Product* current;
    int count = 0;
    if (head == NULL || keyword == NULL || result == NULL || strlen(keyword) == 0) { // validate all input Parameters
        return -1;
    }
    current = head;
    while (current != NULL) {// Count the number of matching Products.
        if (containsIgnoreCase(current->name, keyword)) {
            count++;
        }
        current = current->next;
    }
    result->count = count;
    if (count == 0) {
        result->products = NULL;
        return 0;
    }
    result->products = (Product**)malloc(sizeof(Product*) * count); //Allocate memory for the matching Product pointers.
    if (result->products == NULL) {
        result->count = 0;
        return -1;
    }
    current = head;
    count = 0;
    // Store pointers to all matching Products.
    while (current != NULL) {
        if (containsIgnoreCase(current->name, keyword)) {
            result->products[count++] = current;
        }
        current=current->next;
    }
	return 0;
}
/*Function: search_below_threshold
Purpose:Search the Inventory for Products whose quantity is below the specified threshold.
Inputs:
head - Pointer to the first Product in the Inventory.
threshold - Inventory threshold value.
result - Pointer to a SearchResult structure to store matching Products.
Output:Stores pointers to Products below the specified threshold in the SearchResult structure.
Returns:int - Returns 0 on success or -1 if the input is invalid or memory allocation fails.
Author:Inderpreet Kaur Hundal*/
int search_below_threshold(Product* head, int threshold, SearchResult* result) {
    Product* current;
    int count = 0;
    if (head == NULL || result == NULL || threshold < 0) { //validate all input parameters
        return -1;
    }
    current = head;
    while (current != NULL) {//counts products whose quantity is below the threshold
        if (current->quantity < threshold) {
            count++;
        }
        current = current->next;
    }
    result->count = count;
    if (count == 0) {
        result->products = NULL;
        return 0;
    }
    result->products = (Product**)malloc(sizeof(Product*) * count);// allocate memory for matching products
    if (result->products == NULL) {
        result->count = 0;
        return -1;
    }
    current = head;
    count = 0;
    while (current != NULL) { // stores pointers to matching products
        if (current->quantity < threshold) {
            result->products[count++] = current;
        }
        current = current->next;
    }
	return 0;
}
/*Function: search_by_quantity_range
Purpose:Search the Inventory for Products whose quantities fall within the specified inclusive range.
Inputs:
head - Pointer to the first Product in the Inventory.
minimum - Minimum quantity in the range.
maximum - Maximum quantity in the range.
result - Pointer to a SearchResult structure to store matching Products.
Output:Stores pointers to Products within the specified quantity range.
Returns:int - Returns 0 on success or -1 if the input is invalid or memory allocation fails.
Author:Inderpreet Kaur Hundal*/
int search_by_quantity_range(Product* head, int minimum, int maximum, SearchResult* result) {
    Product* current;
    int count = 0;
    if (head == NULL || result == NULL || minimum>maximum) { // Validate input parameters
        return -1;
    }
    current = head;
    while (current != NULL) { // counts products that fall within the specified range
        if (current->quantity >= minimum && current->quantity <= maximum) {
            count++;
        }
        current = current->next;
    }
    result->count = count;
    if (count == 0) {
        result->products = NULL;
        return 0;
    }
    result->products = (Product**)malloc(sizeof(Product*) * count);// allocate memory for product pointers
    if (result->products == NULL) {
        result->count = 0;
        return -1;
    }
    current = head;
    count = 0;
    while (current != NULL) { //store matching product pointers
        if (current->quantity >= minimum && current->quantity <= maximum) {
            result->products[count++] = current;
        }
        current = current->next;
    }
	return 0;
}
/*Function: search_all_products
Purpose:Retrieve all Products currently stored in the Inventory.
Inputs:
head - Pointer to the first Product in the Inventory.
result - Pointer to a SearchResult structure to store all Products.
Output:Stores pointers to every Product in the Inventory.
Returns:int - Returns 0 on success or -1 if the input is invalid or memory allocation fails.
Author:Inderpreet Kaur Hundal*/
int search_all_products(Product* head, SearchResult* result) {
    Product* current;
    int count = 0;
    if (head == NULL || result == NULL) {// validate input parameters
        return -1;
    }
    current = head;
    while (current != NULL) { // count total products in the inventory
        count++;
        current = current->next;
    }
    result->count = count;
    if (count == 0) {
        result->products = NULL;
        return 0;
    }
    result->products = (Product**)malloc(sizeof(Product*) * count);// allocate memory for product pointers
    if (result->products == NULL) {
        result->count = 0;
        return -1;
    }
    current = head;
    count = 0;
    while (current != NULL) { //store pointers to every product
        result->products[count++] = current;
        current = current->next;
    }
	return 0;
}
/*Function: search_free_results
Purpose:Release dynamically allocated memory used by a SearchResult structure.
Inputs:
result - Pointer to the SearchResult structure.
Output:Releases allocated memory, sets the Product pointer to NULL, and resets the result count to 0.
Returns:No return value.
Author:Inderpreet Kaur Hundal*/
void search_free_results(SearchResult* result) {
    if (result == NULL) {
        return;// Do nothing if the SearchResult pointer is invalid.
    }
    free(result->products);// Release dynamically allocated memory.
    result->products = NULL; //Reset the SearchResult structure
    result->count = 0;
}
/*Function: search_record_history
Purpose:Record a search criterion and its corresponding result count in the recent search history.
Inputs:
criteria - Search text or criteria used for the search.
resultCount - Number of Products returned by the search.
Output:Adds a new history entry while maintaining the maximum history capacity.
Returns:int - Returns 0 on success or -1 if the input is invalid.
Author:Inderpreet Kaur Hundal*/
int search_record_history(const char* criteria, int resultCount) {
    int i;
    if (criteria == NULL) {  //Validate the search critteria
        return -1;
    }
    if (historyCount == SEARCH_HISTORY_CAPACITY) {
        for (i = 1; i < SEARCH_HISTORY_CAPACITY; i++) { // If history is full, remove the oldest entry.
            history[i - 1] = history[i];
        }
        historyCount--;
    }
    strcpy_s(history[historyCount].criteria, SEARCH_TEXT_LENGTH, criteria); //store new search entry
    history[historyCount].resultCount = resultCount;
    historyCount++;
	return 0;
}
/*Function: search_get_history
Purpose:Retrieve the recent search history entries.
Inputs:
entries - Array used to receive the stored search history entries.
Output:Copies the stored history entries into the supplied array.
Returns:int - Returns the number of stored history entries or -1 if the input is invalid.
Author:Inderpreet Kaur Hundal*/
int search_get_history(SearchHistoryEntry entries[]) {
    int i;
    if (entries == NULL) {  // validate the output array 
        return -1;
    }
    for (i = 0; i < historyCount; i++) { // copy every stored history entry
        entries[i] = history[i];
    }
    return historyCount;
}
/*Function: search_clear_history
Purpose:Remove all entries from the recent search history.
Inputs:None.
Output:Clears the stored search history and resets the history count to zero.
Returns:No return value.
Author:Inderpreet Kaur Hundal*/
void search_clear_history(void) {
    historyCount = 0;  // remove all stored history entries
}
