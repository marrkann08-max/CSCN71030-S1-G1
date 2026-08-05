#include "pch.h"
#include "CppUnitTest.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

extern "C"
{
#include "../WarehouseManagementSystem/alerts.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct Product
{
    unsigned int id;
    char name[64];
    int quantity;
    char location[32];
    Product* next;
};

namespace AlertsTests
{
    static std::string read_file(const char* path)
    {
        std::ifstream input(path);
        std::ostringstream contents;
        contents << input.rdbuf();
        return contents.str();
    }

    static void write_file(const char* path, const char* contents)
    {
        std::ofstream output(path, std::ios::trunc);
        output << contents;
    }

    TEST_CLASS(AlertsFunctionalTests)
    {
    public:
        TEST_METHOD(ALR_F_001_SelectOnlyProductsBelowThreshold)
        {
            const char* report_path = "alerts_below_threshold.txt";
            Product third = { 1003U, "Drill Press", 25, "C-03", nullptr };
            Product second = { 1002U, "Safety Gloves", 10, "B-02", &third };
            Product first = { 1001U, "Hammer", 5, "A-01", &second };

            Assert::AreEqual(
                1,
                check_low_stock(&first, 10, report_path)
            );

            const std::string report = read_file(report_path);
            Assert::IsTrue(report.find("Threshold: 10") != std::string::npos);
            Assert::IsTrue(report.find("1001") != std::string::npos);
            Assert::IsTrue(report.find("Hammer") != std::string::npos);
            Assert::IsTrue(report.find("1002") == std::string::npos);
            Assert::IsTrue(report.find("Safety Gloves") == std::string::npos);
            Assert::IsTrue(report.find("1003") == std::string::npos);

            std::remove(report_path);
        }

        TEST_METHOD(ALR_F_002_NoMatchesAndInvalidInputs)
        {
            const char* report_path = "alerts_no_matches.txt";
            Product second = { 1002U, "Drill Press", 25, "B-02", nullptr };
            Product first = { 1001U, "Safety Gloves", 10, "A-01", &second };

            Assert::AreEqual(
                0,
                check_low_stock(&first, 1, report_path)
            );
            Assert::IsTrue(
                read_file(report_path).find(
                    "No products are below the configured threshold."
                ) != std::string::npos
            );

            write_file(report_path, "sentinel-content\n");

            Assert::AreEqual(-1, check_low_stock(&first, 0, report_path));
            Assert::IsTrue(
                read_file(report_path).find("sentinel-content") !=
                std::string::npos
            );

            Assert::AreEqual(-1, check_low_stock(&first, -1, report_path));
            Assert::AreEqual(-1, check_low_stock(&first, 10, nullptr));
            Assert::AreEqual(-1, check_low_stock(&first, 10, ""));
            Assert::AreEqual(-1, check_low_stock(&first, 10, "   "));

            std::remove(report_path);
        }

        TEST_METHOD(ALR_F_003_OverwriteAndEmptyInventory)
        {
            const char* report_path = "alerts_overwrite.txt";
            const char* empty_report_path = "alerts_empty_inventory.txt";
            Product third = { 1003U, "Drill Press", 25, "C-03", nullptr };
            Product second = { 1002U, "Safety Gloves", 10, "B-02", &third };
            Product first = { 1001U, "Hammer", 5, "A-01", &second };

            Assert::AreEqual(
                2,
                check_low_stock(&first, 11, report_path)
            );
            Assert::IsTrue(
                read_file(report_path).find("Safety Gloves") !=
                std::string::npos
            );

            Assert::AreEqual(
                1,
                check_low_stock(&first, 6, report_path)
            );

            const std::string overwritten_report = read_file(report_path);
            Assert::IsTrue(
                overwritten_report.find("Threshold: 6") != std::string::npos
            );
            Assert::IsTrue(
                overwritten_report.find("Hammer") != std::string::npos
            );
            Assert::IsTrue(
                overwritten_report.find("Safety Gloves") == std::string::npos
            );
            Assert::IsTrue(
                overwritten_report.find("Drill Press") == std::string::npos
            );

            Assert::AreEqual(
                0,
                check_low_stock(nullptr, 10, empty_report_path)
            );

            const std::string empty_report = read_file(empty_report_path);
            Assert::IsTrue(
                empty_report.find(
                    "No products are below the configured threshold."
                ) != std::string::npos
            );
            Assert::IsTrue(
                empty_report.find("Product ID:") == std::string::npos
            );

            std::remove(report_path);
            std::remove(empty_report_path);
        }
    };
}
