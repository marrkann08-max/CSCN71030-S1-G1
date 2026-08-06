#include "search.h"
#include "utilities.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static SearchHistoryEntry history[SEARCH_HISTORY_CAPACITY];
static int historyCount = 0;

/**
 * Author: Inderpreet Kaur Hundal
 * Input: Result pointer and number of Product pointers required.
 * Output: Zero with initialized storage; -1 for invalid input/allocation failure.
 * Purpose: Initialize SearchResult consistently for every list query.
 */
static int search_allocate_result(SearchResult* result, int count)
{
    if (util_check_null(result) != 0 || count < 0)
    {
        return -1;
    }

    result->products = NULL;
    result->count = 0;

    if (count == 0)
    {
        return 0;
    }

    result->products = (Product**)malloc(sizeof(Product*) * (size_t)count);

    if (result->products == NULL)
    {
        return -1;
    }

    result->count = count;
    return 0;
}

/**
 * Author: Inderpreet Kaur Hundal
 * Input: Source text and keyword pattern.
 * Output: One when pattern occurs case-insensitively, otherwise zero.
 * Purpose: Perform safe case-insensitive substring comparison.
 */
static int contains_ignore_case(const char* text, const char* pattern)
{
    while (*text != '\0')
    {
        const char* text_cursor = text;
        const char* pattern_cursor = pattern;

        while (*text_cursor != '\0' && *pattern_cursor != '\0' &&
            tolower((unsigned char)*text_cursor) ==
            tolower((unsigned char)*pattern_cursor))
        {
            ++text_cursor;
            ++pattern_cursor;
        }

        if (*pattern_cursor == '\0')
        {
            return 1;
        }

        ++text;
    }

    return 0;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory head and non-zero Product ID.
 * Output: Returns the matching Product pointer, or NULL.
 * Purpose: Find one Product by ID without changing Inventory.
 */
Product* search_by_id(Product* head, unsigned int productID)
{
    if (util_check_null(head) != 0 || productID == 0U)
    {
        return NULL;
    }

    return inventory_get_product(head, productID);
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory, name keyword, and SearchResult pointer.
 * Output: Stores case-insensitive matches and returns 0, otherwise -1.
 * Purpose: Find Products whose names contain the supplied keyword.
 */
int search_by_name(Product* head, const char* keyword, SearchResult* result)
{
    Product* current;
    char validated_keyword[SEARCH_TEXT_LENGTH];
    size_t keyword_length;
    int count = 0;
    int index = 0;

    if (util_check_null(keyword) != 0 || util_check_null(result) != 0)
    {
        return -1;
    }

    keyword_length = strlen(keyword);

    if (keyword_length >= SEARCH_TEXT_LENGTH)
    {
        keyword_length = SEARCH_TEXT_LENGTH - 1U;
    }

    memcpy(validated_keyword, keyword, keyword_length);
    validated_keyword[keyword_length] = '\0';

    if (util_validate_string(validated_keyword, SEARCH_TEXT_LENGTH - 1U) != 0)
    {
        return -1;
    }

    for (current = head; current != NULL; current = current->next)
    {
        if (contains_ignore_case(current->name, validated_keyword))
        {
            ++count;
        }
    }

    if (search_allocate_result(result, count) != 0)
    {
        return -1;
    }

    for (current = head; current != NULL; current = current->next)
    {
        if (contains_ignore_case(current->name, validated_keyword))
        {
            result->products[index++] = current;
        }
    }

    return 0;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory, non-negative threshold, and result pointer.
 * Output: Stores Products below the threshold and returns 0, otherwise -1.
 * Purpose: Find low-stock Products without changing Inventory.
 */
int search_below_threshold(Product* head, int threshold, SearchResult* result)
{
    Product* current;
    int count = 0;
    int index = 0;

    if (util_check_null(result) != 0 || threshold < 0)
    {
        return -1;
    }

    for (current = head; current != NULL; current = current->next)
    {
        if (current->quantity < threshold)
        {
            ++count;
        }
    }

    if (search_allocate_result(result, count) != 0)
    {
        return -1;
    }

    for (current = head; current != NULL; current = current->next)
    {
        if (current->quantity < threshold)
        {
            result->products[index++] = current;
        }
    }

    return 0;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory, inclusive quantity range, and result pointer.
 * Output: Stores matching Products and returns 0, otherwise -1.
 * Purpose: Find Products inside a validated quantity range.
 */
int search_by_quantity_range(
    Product* head,
    int minimum,
    int maximum,
    SearchResult* result
)
{
    Product* current;
    int count = 0;
    int index = 0;

    if (util_check_null(result) != 0 ||
        minimum < 0 || maximum < 0 || minimum > maximum)
    {
        return -1;
    }

    for (current = head; current != NULL; current = current->next)
    {
        if (current->quantity >= minimum && current->quantity <= maximum)
        {
            ++count;
        }
    }

    if (search_allocate_result(result, count) != 0)
    {
        return -1;
    }

    for (current = head; current != NULL; current = current->next)
    {
        if (current->quantity >= minimum && current->quantity <= maximum)
        {
            result->products[index++] = current;
        }
    }

    return 0;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory head and SearchResult pointer.
 * Output: Stores every Product pointer and returns 0, otherwise -1.
 * Purpose: Return a non-owning snapshot of all Inventory Products.
 */
int search_all_products(Product* head, SearchResult* result)
{
    Product* current;
    int count = 0;
    int index = 0;

    if (util_check_null(result) != 0)
    {
        return -1;
    }

    for (current = head; current != NULL; current = current->next)
    {
        ++count;
    }

    if (search_allocate_result(result, count) != 0)
    {
        return -1;
    }

    for (current = head; current != NULL; current = current->next)
    {
        result->products[index++] = current;
    }

    return 0;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: SearchResult pointer, which may be NULL.
 * Output: Frees Search-owned memory and resets the result fields.
 * Purpose: Release result storage without freeing Inventory Products.
 */
void search_free_results(SearchResult* result)
{
    if (result == NULL)
    {
        return;
    }

    free(result->products);
    result->products = NULL;
    result->count = 0;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Search description and non-negative result count.
 * Output: Records the search and returns 0, otherwise -1.
 * Purpose: Store the most recent validated Search summaries.
 */
int search_record_history(const char* criteria, int resultCount)
{
    char validated_criteria[SEARCH_TEXT_LENGTH];
    size_t criteria_length;
    int index;

    if (util_check_null(criteria) != 0 || resultCount < 0)
    {
        return -1;
    }

    criteria_length = strlen(criteria);

    if (criteria_length >= SEARCH_TEXT_LENGTH)
    {
        criteria_length = SEARCH_TEXT_LENGTH - 1U;
    }

    memcpy(validated_criteria, criteria, criteria_length);
    validated_criteria[criteria_length] = '\0';

    if (util_validate_string(validated_criteria, SEARCH_TEXT_LENGTH - 1U) != 0)
    {
        return -1;
    }

    if (historyCount == SEARCH_HISTORY_CAPACITY)
    {
        for (index = 1; index < SEARCH_HISTORY_CAPACITY; ++index)
        {
            history[index - 1] = history[index];
        }

        --historyCount;
    }

    memcpy(
        history[historyCount].criteria,
        validated_criteria,
        strlen(validated_criteria) + 1U
    );
    history[historyCount].resultCount = resultCount;
    ++historyCount;
    return 0;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Array with space for the Search history entries.
 * Output: Copies the history and returns its count, otherwise -1.
 * Purpose: Let Main display Search history without exposing internal state.
 */
int search_get_history(SearchHistoryEntry entries[])
{
    int index;

    if (util_check_null(entries) != 0)
    {
        return -1;
    }

    for (index = 0; index < historyCount; ++index)
    {
        entries[index] = history[index];
    }

    return historyCount;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Current module-local Search history.
 * Output: Erases all stored history entries and resets the count.
 * Purpose: Clear Search history safely and deterministically.
 */
void search_clear_history(void)
{
    memset(history, 0, sizeof(history));
    historyCount = 0;
}
