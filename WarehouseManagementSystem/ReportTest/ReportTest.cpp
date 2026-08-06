#include "pch.h"
#include "CppUnitTest.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

extern "C" {
#include "../WarehouseManagementSystem/inventory.h"
#include "../WarehouseManagementSystem/Report.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;

namespace ReportTest
{
    TEST_CLASS(ReportTests)
    {
        fs::path outputDirectory;

        std::vector<fs::path> filesWithExtension(const wchar_t* extension)
        {
            std::vector<fs::path> files;
            for (const auto& entry : fs::directory_iterator(outputDirectory)) {
                if (entry.is_regular_file() && entry.path().extension() == extension) {
                    files.push_back(entry.path());
                }
            }
            return files;
        }

        static std::string readFile(const fs::path& path)
        {
            std::ifstream input(path, std::ios::binary);
            std::ostringstream content;
            content << input.rdbuf();
            return content.str();
        }

    public:
        TEST_METHOD_INITIALIZE(Initialize)
        {
            outputDirectory = fs::current_path() / "report_test_output";
            fs::remove_all(outputDirectory);
            fs::create_directory(outputDirectory);
        }

        TEST_METHOD_CLEANUP(Cleanup)
        {
            fs::remove_all(outputDirectory);
        }

        TEST_METHOD(TextReportContainsInventoryWithoutChangingIt)
        {
            Product* head = nullptr;
            Assert::AreEqual(0, inventory_add_product(&head, 1001U, "Hammer", 5, "A-01"));
            Assert::AreEqual(0, inventory_add_product(&head, 2002U, "Nails", 10, "B-02"));

            const std::string directory = outputDirectory.string();
            Assert::AreEqual(0, report_generate_text(&head, directory.c_str()));

            const auto files = filesWithExtension(L".txt");
            Assert::AreEqual(static_cast<size_t>(1), files.size());
            const std::string content = readFile(files.front());
            Assert::IsTrue(content.find("Warehouse Inventory Report") != std::string::npos);
            Assert::IsTrue(content.find("1001 | Hammer | 5 | A-01") != std::string::npos);
            Assert::IsTrue(content.find("2002 | Nails | 10 | B-02") != std::string::npos);
            Assert::AreEqual(1001U, head->id);
            Assert::AreEqual(2002U, head->next->id);

            inventory_free_all(&head);
        }

        TEST_METHOD(CsvReportEscapesSpecialCharacters)
        {
            Product* head = nullptr;
            Assert::AreEqual(0, inventory_add_product(
                &head, 4004U, "Safety, \"Gloves\"", 20, "A,01"));

            const std::string directory = outputDirectory.string();
            Assert::AreEqual(0, report_generate_csv(&head, directory.c_str()));

            const auto files = filesWithExtension(L".csv");
            Assert::AreEqual(static_cast<size_t>(1), files.size());
            const std::string content = readFile(files.front());
            Assert::IsTrue(content.find("Product ID,Name,Quantity,Location") != std::string::npos);
            Assert::IsTrue(content.find("4004,\"Safety, \"\"Gloves\"\"\",20,\"A,01\"") != std::string::npos);

            inventory_free_all(&head);
        }

        TEST_METHOD(EmptyInventoryProducesHeaderOnlyReport)
        {
            Product* head = nullptr;
            const std::string directory = outputDirectory.string();

            Assert::AreEqual(0, report_generate_csv(&head, directory.c_str()));
            const auto files = filesWithExtension(L".csv");
            Assert::AreEqual(static_cast<size_t>(1), files.size());
            Assert::AreEqual(std::string("Product ID,Name,Quantity,Location\r\n"),
                readFile(files.front()));
        }

        TEST_METHOD(InvalidInputsAreRejected)
        {
            Product* head = nullptr;
            Assert::AreEqual(-1, report_generate_text(nullptr, outputDirectory.string().c_str()));
            Assert::AreEqual(-1, report_generate_text(&head, nullptr));
            Assert::AreEqual(-1, report_generate_text(&head, ""));
            Assert::AreEqual(-1, report_generate_text(&head, "   "));
            Assert::AreEqual(-1, report_generate_csv(&head, "missing-report-directory"));
            Assert::AreEqual(static_cast<size_t>(0), filesWithExtension(L".txt").size());
            Assert::AreEqual(static_cast<size_t>(0), filesWithExtension(L".csv").size());
        }

        TEST_METHOD(BackToBackReportsUseUniqueNames)
        {
            Product* head = nullptr;
            const std::string directory = outputDirectory.string();

            Assert::AreEqual(0, report_generate_text(&head, directory.c_str()));
            Assert::AreEqual(0, report_generate_text(&head, directory.c_str()));
            Assert::AreEqual(static_cast<size_t>(2), filesWithExtension(L".txt").size());
        }
    };
}
