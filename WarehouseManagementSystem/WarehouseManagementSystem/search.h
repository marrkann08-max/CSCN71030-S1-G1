#pragma once
#ifndef SEARCH_H
#define SEARCH_H
#define PRODUCT_NAME_LENGTH 64
#define PRODUCT_LOCATION_LENGTH 32
#define SEARCH_HISTORY_CAPACITY 10
#define SEARCH_TEXT_LENGTH 64
typedef struct
{
    char criteria[SEARCH_TEXT_LENGTH];
    int resultCount;
} SearchHistoryEntry;
typedef struct Product
{
    unsigned int id;
    char name[PRODUCT_NAME_LENGTH];
    int quantity;
    char location[PRODUCT_LOCATION_LENGTH];
    struct Product* next;
} Product;
typedef struct {
	Product** products;
	int count;
}SearchResult;
Product* search_by_id(Product* head, unsigned int productID);
int search_by_name(Product* head, const char* keyword, SearchResult* result);
int search_below_threshold(Product* head, int threshold, SearchResult* result);
int search_by_quantity_range(Product* head, int minimum, int maximum, SearchResult* result);
int search_all_products(Product* head, SearchResult* result);
void search_free_results(SearchResult* result);
int search_record_history(const char* criteria,int resultCount);
int search_get_history(SearchHistoryEntry entries[]);
void search_clear_history(void);
#endif