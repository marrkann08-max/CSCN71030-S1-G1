#include<stdio.h>
#include<stdlib.h>
#include"search.h"
static SearchHistoryEntry history[SEARCH_HISTORY_CAPACITY];
static int historyCount = 0;
Product* search_by_id(Product* head, unsigned int productID) {
	return NULL;
}
int search_by_name(Product* head, const char* keyword, SearchResult* result) {
	return 0;
}
int search_below_threshold(Product* head, int threshold, SearchResult* result) {
	return 0;
}
int search_by_quantity_range(Product* head, int minimum, int maximum, SearchResult* result) {
	return 0;
}
int search_all_products(Product* head, SearchResult* result) {
	return 0;
}
void search_free_results(SearchResult* result) {

}
int search_record_history(const char* criteria, int resultCount) {
	return 0;
}
int search_get_history(SearchHistoryEntry history[]) {
	return 0;
}
void search_clear_history(void) {

}