#include "pch.h"
#include "CppUnitTest.h"

extern "C" {
#pragma warning(disable : 4996)
#include "../WarehouseManagementSystem/inventory.h"
#include "../WarehouseManagementSystem/inventory.c"
#include "../WarehouseManagementSystem/Report.h"
#include "../WarehouseManagementSystem/Report.c"
#include "../WarehouseManagementSystem/utilities.h"
#include "../WarehouseManagementSystem/utilities.c"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ReportTest
{
	TEST_CLASS(ReportTest)
	{
	public:

		TEST_METHOD(TextReportNormal)
		{
			Product* head = NULL;

			inventory_add_product(&head, 1001U, "Hammer", 5, "A-01");
			inventory_add_product(&head, 2002U, "Nail", 10, "B-02");
			inventory_add_product(&head, 3003U, "Screw", 15, "C-03");
			inventory_add_product(&head, 4004U, "STUFF", 20, "D-04");

			Assert::AreEqual(0, report_generate_text(&head, ""));
		}
		TEST_METHOD(CSVReportNormal)
		{
			Product* head = NULL;

			inventory_add_product(&head, 4004U, "STUFF", 20, "D-04");
			inventory_add_product(&head, 2002U, "Nail", 10, "B-02");
			inventory_add_product(&head, 1001U, "Hammer", 5, "A-01");
			inventory_add_product(&head, 3003U, "Screw", 15, "C-03");

			Assert::AreEqual(0, report_generate_csv(&head, ""));

		}
		TEST_METHOD(ReportFileNotOpen)
		{
			Product* head = NULL;
			inventory_add_product(&head, 4004U, "STUFF", 20, "D-04");
			Assert::AreEqual(-1, report_generate_csv(&head, "/"));
		}
	};
}
