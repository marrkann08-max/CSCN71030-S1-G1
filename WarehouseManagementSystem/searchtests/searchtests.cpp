#include "pch.h"
#include "CppUnitTest.h"
extern "C" {
#include"../WarehouseManagementSystem/search.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace searchtests
{
	TEST_CLASS(searchtests)
	{
	public:
		
		TEST_METHOD(test_search_existing_product)
		{
			Product product = { 1001,"Hammer",5,"A-01",NULL };
			Product* result = search_by_id(&product, 1001);
			Assert::IsNotNull(result);
			Assert::AreEqual((unsigned int)1001, result->id);
			Assert::AreEqual(5, result->quantity);
		}
		TEST_METHOD(test_search_missing_product)
		{
			Product product = { 1001,"Hammer",5,"A-01",NULL };
			Product* result = search_by_id(&product, 9999);
			Assert::IsNull(result);
		}
		TEST_METHOD(test_search_invalid_product_id)
		{
			Product product = { 1001,"Hammer",5,"A-01",NULL };
			Product* result = search_by_id(&product, 0);
			Assert::IsNull(result);
		}
		TEST_METHOD(test_search_empty_inventory)
		{
			Product* result = search_by_id(NULL, 1001);
			Assert::IsNull(result);
		}
		TEST_METHOD(test_search_partial_name)
		{
			Product p2 = { 1002, "Safety Gloves", 12, "A-02", NULL };
			Product p1 = { 1001, "Hammer", 5, "A-01", &p2 };
			SearchResult result = { NULL, 0 };
			int status = search_by_name(&p1, "ham", &result);
			Assert::AreEqual(0, status);
			Assert::AreEqual(1, result.count);
			Assert::AreEqual((unsigned int)1001, result.products[0]->id);
			search_free_results(&result);
		}
		TEST_METHOD(test_search_case_insensitive)
		{
			Product p2 = { 1002, "Safety Gloves", 12, "A-02", NULL };
			Product p1 = { 1001, "Hammer", 5, "A-01", &p2 };
			SearchResult result = { NULL, 0 };
			int status = search_by_name(&p1, "HAMMER", &result);
			Assert::AreEqual(0, status);
			Assert::AreEqual(1, result.count);
			search_free_results(&result);
		}
		TEST_METHOD(test_search_no_match)
		{
			Product product = { 1001,"Hammer",5,"A-01",NULL };
			SearchResult result = { NULL, 0 };
			int status = search_by_name(&product, "Laptop", &result);
			Assert::AreEqual(0, status);
			Assert::AreEqual(0, result.count);
			search_free_results(&result);
		}
		TEST_METHOD(test_search_empty_keyword)
		{
			Product product = { 1001,"Hammer",5,"A-01",NULL };
			SearchResult result = { NULL, 0 };
			int status = search_by_name(&product, "", &result);
			Assert::AreEqual(-1, status);
		}
		TEST_METHOD(test_search_below_threshold)
		{
			Product p4 = { 1004, "Drill", 10, "A-04", NULL };
			Product p3 = { 1003, "Cable Box", 0, "A-03", &p4 };
			Product p2 = { 1002, "Safety Gloves", 12, "A-02", &p3 };
			Product p1 = { 1001, "Hammer", 5, "A-01", &p2 };
			SearchResult result = { NULL, 0 };
			int status = search_below_threshold(&p1, 10, &result);
			Assert::AreEqual(0, status);
			Assert::AreEqual(2, result.count);
			Assert::AreEqual((unsigned int)1001, result.products[0]->id);
			Assert::AreEqual((unsigned int)1003, result.products[1]->id);
			search_free_results(&result);
		}
		TEST_METHOD(test_search_quantity_range)
		{
			Product p4 = { 1004, "Drill", 10, "A-04", NULL };
			Product p3 = { 1003, "Cable Box", 0, "A-03", &p4 };
			Product p2 = { 1002, "Safety Gloves", 12, "A-02", &p3 };
			Product p1 = { 1001, "Hammer", 5, "A-01", &p2 };
			SearchResult result = { NULL, 0 };
			int status = search_by_quantity_range(&p1, 5, 12, &result);
			Assert::AreEqual(0, status);
			Assert::AreEqual(3, result.count);
			search_free_results(&result);
		}
		TEST_METHOD(test_search_all_products)
		{
			Product p4 = { 1004, "Drill", 10, "A-04", NULL };
			Product p3 = { 1003, "Cable Box", 0, "A-03", &p4 };
			Product p2 = { 1002, "Safety Gloves", 12, "A-02", &p3 };
			Product p1 = { 1001, "Hammer", 5, "A-01", &p2 };
			SearchResult result = { NULL, 0 };
			int status = search_all_products(&p1, &result);
			Assert::AreEqual(0, status);
			Assert::AreEqual(4, result.count);
			search_free_results(&result);
		}
		TEST_METHOD(test_history_initially_empty)
		{
			search_clear_history();
			SearchHistoryEntry history[SEARCH_HISTORY_CAPACITY];
			int count = search_get_history(history);
			Assert::AreEqual(0, count);
		}
		TEST_METHOD(test_record_history)
		{
			search_clear_history();
			search_record_history("hammer", 1);
			search_record_history("gloves", 2);
			SearchHistoryEntry history[SEARCH_HISTORY_CAPACITY];
			int count = search_get_history(history);
			Assert::AreEqual(2, count);
			Assert::AreEqual(std::string("hammer"), std::string(history[0].criteria));
			Assert::AreEqual(1, history[0].resultCount);
		}
		TEST_METHOD(test_history_capacity)
		{
			search_clear_history();
			for (int i = 0; i < SEARCH_HISTORY_CAPACITY + 2; i++)
			{
				char text[20];
				sprintf_s(text, "Search%d", i);
				search_record_history(text, i);
			}
			SearchHistoryEntry history[SEARCH_HISTORY_CAPACITY];
			int count = search_get_history(history);
			Assert::AreEqual(SEARCH_HISTORY_CAPACITY, count);
			Assert::AreEqual(std::string("Search2"), std::string(history[0].criteria));
		}
		TEST_METHOD(test_clear_history)
		{
			search_record_history("Hammer", 1);
			search_clear_history();
			SearchHistoryEntry history[SEARCH_HISTORY_CAPACITY];
			int count = search_get_history(history);
			Assert::AreEqual(0, count);
		}


	};
}
