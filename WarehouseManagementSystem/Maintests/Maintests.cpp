//--------------------------------------------------------------------------
// Maintests.cpp - Functional tests for the Main module
//                    Author: Ivan Immanuel Shaji
//--------------------------------------------------------------------------

#include "pch.h"
#include "CppUnitTest.h"

#include "../WarehouseManagementSystem/main.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MainTests
{
	TEST_CLASS(MainMenuTests)
	{
	public:

		/*
		 * Author: Ivan Immanuel Shaji
		 * Input: Valid menu strings.
		 * Output: Verifies the corresponding MenuChoice values.
		 * Purpose: Test valid menu conversion.
		 */
		TEST_METHOD(MN_F_001_ValidMenuChoices)
		{
			char exit_text[] = "0";
			char inventory_text[] = "1";
			char reports_text[] = " 4 ";
			char alerts_text[] = "5";
			MenuChoice choice = MENU_EXIT;

			Assert::AreEqual(0, parse_menu_choice(exit_text, &choice));
			Assert::AreEqual((int)MENU_EXIT, (int)choice);

			Assert::AreEqual(0, parse_menu_choice(inventory_text, &choice));
			Assert::AreEqual((int)MENU_INVENTORY, (int)choice);

			Assert::AreEqual(0, parse_menu_choice(reports_text, &choice));
			Assert::AreEqual((int)MENU_REPORTS, (int)choice);

			Assert::AreEqual(0, parse_menu_choice(alerts_text, &choice));
			Assert::AreEqual((int)MENU_ALERTS, (int)choice);
		}

		/*
		 * Author: Ivan Immanuel Shaji
		 * Input: Invalid text and out-of-range menu values.
		 * Output: Verifies that each invalid selection is rejected.
		 * Purpose: Test menu validation failures.
		 */
		TEST_METHOD(MN_F_002_InvalidMenuChoices)
		{
			char alphabetic[] = "abc";
			char negative[] = "-1";
			char too_large[] = "6";
			char trailing_text[] = "1abc";
			char empty[] = "";
			MenuChoice choice = MENU_ORDERS;

			Assert::AreEqual(-1, parse_menu_choice(alphabetic, &choice));
			Assert::AreEqual(-1, parse_menu_choice(negative, &choice));
			Assert::AreEqual(-1, parse_menu_choice(too_large, &choice));
			Assert::AreEqual(-1, parse_menu_choice(trailing_text, &choice));
			Assert::AreEqual(-1, parse_menu_choice(empty, &choice));

			Assert::AreEqual((int)MENU_ORDERS, (int)choice);
		}

		/*
		 * Author: Ivan Immanuel Shaji
		 * Input: Null input and output pointers.
		 * Output: Verifies safe rejection without crashing.
		 * Purpose: Test Main pointer validation.
		 */
		TEST_METHOD(MN_F_003_NullPointers)
		{
			char valid[] = "1";
			MenuChoice choice = MENU_EXIT;

			Assert::AreEqual(-1, parse_menu_choice(nullptr, &choice));
			Assert::AreEqual(-1, parse_menu_choice(valid, nullptr));
		}

		/*
		 * Author: Ivan Immanuel Shaji
		 * Input: Exit, empty-Alerts, invalid-choice, and null dispatch inputs.
		 * Output: Verifies safe dispatch without interactive input or a crash.
		 * Purpose: Test Main's module-routing boundary and pointer validation.
		 */
		TEST_METHOD(MN_F_004_DispatchValidation)
		{
			Config config = { 10, 2026U };
			Product* head = nullptr;

			Assert::AreEqual(
				0,
				dispatch_menu_option((int)MENU_EXIT, &head, &config)
			);
			Assert::AreEqual(
				0,
				dispatch_menu_option((int)MENU_ALERTS, &head, &config)
			);
			Assert::AreEqual(-1, dispatch_menu_option(99, &head, &config));
			Assert::AreEqual(
				-1,
				dispatch_menu_option((int)MENU_EXIT, nullptr, &config)
			);
			Assert::AreEqual(
				-1,
				dispatch_menu_option((int)MENU_EXIT, &head, nullptr)
			);
		}
	};
}
