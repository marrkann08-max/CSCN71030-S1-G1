#ifndef SEARCH_H
#define SEARCH_H

#include "inventory.h"

#define SEARCH_HISTORY_CAPACITY 10
#define SEARCH_TEXT_LENGTH 64

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    char criteria[SEARCH_TEXT_LENGTH];
    int resultCount;
} SearchHistoryEntry;

typedef struct
{
    Product** products;
    int count;
} SearchResult;

/**
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory head and a non-zero Product ID.
 * Output: Matching shared Product pointer, or NULL when invalid/not found.
 * Purpose: Locate one Product without copying or modifying Inventory data.
 */
Product* search_by_id(Product* head, unsigned int productID);

/**
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory head, name keyword, and result destination.
 * Output: Zero on success with case-insensitive matches; -1 for invalid input.
 * Purpose: Find Products whose names contain a validated keyword.
 */
int search_by_name(Product* head, const char* keyword, SearchResult* result);

/**
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory head, non-negative threshold, and result destination.
 * Output: Zero with Products strictly below threshold; -1 for invalid input.
 * Purpose: Find low-stock Products without changing Inventory.
 */
int search_below_threshold(Product* head, int threshold, SearchResult* result);

/**
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory head, inclusive non-negative range, and destination.
 * Output: Zero with matching Products; -1 for an invalid range/destination.
 * Purpose: Find Products by an inclusive quantity interval.
 */
int search_by_quantity_range(
    Product* head,
    int minimum,
    int maximum,
    SearchResult* result
);

/**
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory head (which may be empty) and result destination.
 * Output: Zero with every Product pointer; -1 for an invalid destination.
 * Purpose: Return a non-owning snapshot of the shared Inventory list.
 */
int search_all_products(Product* head, SearchResult* result);

/**
 * Author: Inderpreet Kaur Hundal
 * Input: SearchResult pointer, which may be NULL.
 * Output: Frees only the pointer array and resets products/count.
 * Purpose: Release Search-owned memory without freeing shared Products.
 */
void search_free_results(SearchResult* result);

/**
 * Author: Inderpreet Kaur Hundal
 * Input: Search criterion text and a non-negative result count.
 * Output: Zero after recording; -1 for invalid input.
 * Purpose: Store up to the ten most recent validated search summaries.
 */
int search_record_history(const char* criteria, int resultCount);

/**
 * Author: Inderpreet Kaur Hundal
 * Input: Array with capacity for SEARCH_HISTORY_CAPACITY entries.
 * Output: Copies history and returns its count, or -1 for NULL.
 * Purpose: Retrieve recent searches without exposing internal storage.
 */
int search_get_history(SearchHistoryEntry entries[]);

/**
 * Author: Inderpreet Kaur Hundal
 * Input: None.
 * Output: No return value; stored history is erased.
 * Purpose: Reset Search history deterministically and safely.
 */
void search_clear_history(void);

#ifdef __cplusplus
}
#endif

#endif
