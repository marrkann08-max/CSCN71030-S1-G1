//--------------------------------------------------------------------------
// ReportTest.cpp - Functional tests for the Reports module
//                  Authors: Alex English and Ivan Immanuel Shaji
//--------------------------------------------------------------------------

#include "pch.h"
#include "CppUnitTest.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

extern "C" {
#include "../WarehouseManagementSystem/inventory.h"
#include "../WarehouseManagementSystem/Report.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;

namespace ReportTest
{
    /*
     * Authors: Alex English and Ivan Immanuel Shaji
     * Input: Path to an existing report file.
     * Output: Returns the complete file contents as a string.
     * Purpose: Verify report output without altering production code.
     */
    static std::string read_file(const fs::path& path)
    {
        std::ifstream input(path, std::ios::binary);
        std::ostringstream content;

        Assert::IsTrue(input.is_open());
        content << input.rdbuf();
        return content.str();
    }

    TEST_CLASS(ReportTests)
    {
        fs::path output_directory;
        fs::path text_path;
        fs::path csv_path;

    public:
        /*
         * Authors: Alex English and Ivan Immanuel Shaji
         * Input: Current test working directory.
         * Output: Creates a clean temporary report directory and file paths.
         * Purpose: Isolate each Reports test from prior file-system state.
         */
        TEST_METHOD_INITIALIZE(Initialize)
        {
            output_directory = fs::current_path() / "report_test_output";
            fs::remove_all(output_directory);
            fs::create_directory(output_directory);
            text_path = output_directory / "inventory_report.txt";
            csv_path = output_directory / "inventory_report.csv";
        }

        /*
         * Authors: Alex English and Ivan Immanuel Shaji
         * Input: Temporary report directory created for the current test.
         * Output: Removes every generated test report.
         * Purpose: Leave no report artifacts after test execution.
         */
        TEST_METHOD_CLEANUP(Cleanup)
        {
            fs::remove_all(output_directory);
        }

        /*
         * Authors: Alex English and Ivan Immanuel Shaji
         * Input: Two inventory products and an explicit text-file path.
         * Output: Verifies timestamped content and unchanged inventory fields.
         * Purpose: Test the complete REQ-RPT text-report workflow.
         */
        TEST_METHOD(RPT_F_001_TextReport)
        {
            Product* head = nullptr;
            const std::string path = text_path.string();

            Assert::AreEqual(0, inventory_add_product(&head, 1001U, "Hammer", 5, "A-01"));
            Assert::AreEqual(0, inventory_add_product(&head, 2002U, "Nails", 10, "B-02"));
            Assert::AreEqual(0, report_generate_text(head, path.c_str()));

            const std::string content = read_file(text_path);
            Assert::IsTrue(content.find("Warehouse Inventory Report") != std::string::npos);
            Assert::IsTrue(content.find("Generated: ") != std::string::npos);
            Assert::IsTrue(content.find("1001 | Hammer | 5 | A-01") != std::string::npos);
            Assert::IsTrue(content.find("2002 | Nails | 10 | B-02") != std::string::npos);
            Assert::AreEqual(1001U, head->id);
            Assert::AreEqual(2002U, head->next->id);

            inventory_free_all(&head);
        }

        /*
         * Authors: Alex English and Ivan Immanuel Shaji
         * Input: Product fields containing CSV separator and quote characters.
         * Output: Verifies a correctly escaped CSV record at the requested path.
         * Purpose: Test safe CSV export of valid Inventory data.
         */
        TEST_METHOD(RPT_F_002_CsvEscaping)
        {
            Product* head = nullptr;
            const std::string path = csv_path.string();

            Assert::AreEqual(0, inventory_add_product(
                &head,
                4004U,
                "Safety, \"Gloves\"",
                20,
                "A,01"
            ));
            Assert::AreEqual(0, report_generate_csv(head, path.c_str()));

            const std::string content = read_file(csv_path);
            Assert::IsTrue(content.find("Product ID,Name,Quantity,Location") != std::string::npos);
            Assert::IsTrue(content.find(
                "4004,\"Safety, \"\"Gloves\"\"\",20,\"A,01\""
            ) != std::string::npos);

            inventory_free_all(&head);
        }

        /*
         * Authors: Alex English and Ivan Immanuel Shaji
         * Input: An empty inventory and explicit output paths.
         * Output: Verifies that valid header-only reports are created.
         * Purpose: Define safe Reports behavior when Inventory has no products.
         */
        TEST_METHOD(RPT_F_003_EmptyInventory)
        {
            const std::string text_file = text_path.string();
            const std::string csv_file = csv_path.string();

            Assert::AreEqual(0, report_generate_text(nullptr, text_file.c_str()));
            Assert::AreEqual(0, report_generate_csv(nullptr, csv_file.c_str()));
            Assert::IsTrue(read_file(text_path).find("Generated: ") != std::string::npos);
            Assert::AreEqual(
                std::string("Product ID,Name,Quantity,Location\r\n"),
                read_file(csv_path)
            );
        }

        /*
         * Authors: Alex English and Ivan Immanuel Shaji
         * Input: Null, empty, whitespace-only, overlength, and invalid paths.
         * Output: Verifies rejection without creating an output file.
         * Purpose: Test Reports input validation and file-open failure handling.
         */
        TEST_METHOD(RPT_F_004_InvalidPaths)
        {
            std::string overlength_path(300U, 'x');

            Assert::AreEqual(-1, report_generate_text(nullptr, nullptr));
            Assert::AreEqual(-1, report_generate_text(nullptr, ""));
            Assert::AreEqual(-1, report_generate_text(nullptr, "   "));
            Assert::AreEqual(-1, report_generate_text(nullptr, overlength_path.c_str()));
            Assert::AreEqual(-1, report_generate_csv(
                nullptr,
                "missing-report-directory\\report.csv"
            ));
            Assert::IsFalse(fs::exists(text_path));
            Assert::IsFalse(fs::exists(csv_path));
        }

        /*
         * Authors: Alex English and Ivan Immanuel Shaji
         * Input: Two inventories written successively to one requested file path.
         * Output: Verifies that the second complete snapshot replaces the first.
         * Purpose: Confirm deterministic caller-controlled report destinations.
         */
        TEST_METHOD(RPT_F_005_RequestedPathIsReplaced)
        {
            Product* first = nullptr;
            Product* second = nullptr;
            const std::string path = text_path.string();

            Assert::AreEqual(0, inventory_add_product(&first, 1U, "Old", 1, "A-01"));
            Assert::AreEqual(0, inventory_add_product(&second, 2U, "Current", 2, "B-02"));
            Assert::AreEqual(0, report_generate_text(first, path.c_str()));
            Assert::AreEqual(0, report_generate_text(second, path.c_str()));

            const std::string content = read_file(text_path);
            Assert::IsTrue(content.find("2 | Current | 2 | B-02") != std::string::npos);
            Assert::IsTrue(content.find("1 | Old | 1 | A-01") == std::string::npos);

            inventory_free_all(&first);
            inventory_free_all(&second);
        }
    };
}
