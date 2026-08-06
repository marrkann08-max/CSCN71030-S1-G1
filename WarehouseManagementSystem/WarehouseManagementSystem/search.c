#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"search.h"
static SearchHistoryEntry history[SEARCH_HISTORY_CAPACITY];
static int historyCount = 0;
Product* search_by_id(Product* head, unsigned int productID) {
    Product* current = head;

    if (head == NULL || productID == 0)
    {
	return NULL;
}

    while (current != NULL)
    {
        if (current->id == productID)
        {
            return current;
        }

        current = current->next;
    }
	return NULL;
}
static int containsIgnoreCase(const char* text, const char* pattern) {
    while (*text)
    {
        const char* t = text;
        const char* p = pattern;
        while (*t && *p && tolower((unsigned char)*t) == tolower((unsigned char)*p)) {
            t++;
            p++;
        }
        if (*p == '\0') {
            return 1;
        }
        text++;
    }
    return 0;
}
int search_by_name(Product* head, const char* keyword, SearchResult* result) {
    Product* current;
    int count = 0;
    if (head == NULL || keyword == NULL || result == NULL || strlen(keyword) == 0) {
        return -1;
    }
    current = head;
    while (current != NULL) {
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
    result->products = (Product**)malloc(sizeof(Product*) * count);
    if (result->products == NULL) {
        result->count = 0;
        return -1;
    }
    current = head;
    count = 0;
    while (current != NULL) {
        if (containsIgnoreCase(current->name, keyword)) {
            result->products[count++] = current;
        }
        current=current->next;
    }
	return 0;
}
int search_below_threshold(Product* head, int threshold, SearchResult* result) {
    Product* current;
    int count = 0;
    if (head == NULL || result == NULL || threshold < 0) {
        return -1;
    }
    current = head;
    while (current != NULL) {
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
    result->products = (Product**)malloc(sizeof(Product*) * count);
    if (result->products == NULL) {
        result->count = 0;
        return -1;
    }
    current = head;
    count = 0;
    while (current != NULL) {
        if (current->quantity < threshold) {
            result->products[count++] = current;
        }
        current = current->next;
    }
	return 0;
}
int search_by_quantity_range(Product* head, int minimum, int maximum, SearchResult* result) {
    Product* current;
    int count = 0;
    if (head == NULL || result == NULL || minimum>maximum) {
        return -1;
    }
    current = head;
    while (current != NULL) {
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
    result->products = (Product**)malloc(sizeof(Product*) * count);
    if (result->products == NULL) {
        result->count = 0;
        return -1;
    }
    current = head;
    count = 0;
    while (current != NULL) {
        if (current->quantity >= minimum && current->quantity <= maximum) {
            result->products[count++] = current;
        }
        current = current->next;
    }
	return 0;
}
int search_all_products(Product* head, SearchResult* result) {
    Product* current;
    int count = 0;
    if (head == NULL || result == NULL) {
        return -1;
    }
    current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    result->count = count;
    if (count == 0) {
        result->products = NULL;
        return 0;
    }
    result->products = (Product**)malloc(sizeof(Product*) * count);
    if (result->products == NULL) {
        result->count = 0;
        return -1;
    }
    current = head;
    count = 0;
    while (current != NULL) {
        result->products[count++] = current;
        current = current->next;
    }
	return 0;
}
void search_free_results(SearchResult* result) {
    if (result == NULL) {
        return;
    }
    free(result->products);
    result->products = NULL;
    result->count = 0;
}
int search_record_history(const char* criteria, int resultCount) {
    int i;
    if (criteria == NULL) {
        return -1;
    }
    if (historyCount == SEARCH_HISTORY_CAPACITY) {
        for (i = 1; i < SEARCH_HISTORY_CAPACITY; i++) {
            history[i - 1] = history[i];
        }
        historyCount--;
    }
    strcpy_s(history[historyCount].criteria, SEARCH_TEXT_LENGTH, criteria);
    history[historyCount].resultCount = resultCount;
    historyCount++;
	return 0;
}
int search_get_history(SearchHistoryEntry entries[]) {
    int i;
    if (entries == NULL) {
        return -1;
    }
    for (i = 0; i < historyCount; i++) {
        entries[i] = history[i];
    }
    return historyCount;
}
void search_clear_history(void) {
    historyCount = 0;
}
