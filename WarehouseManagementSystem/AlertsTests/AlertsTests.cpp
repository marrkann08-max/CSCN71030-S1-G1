#include "pch.h"
#include "CppUnitTest.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <io.h>
#include <sstream>
#include <string>

extern "C"
{
#include "../WarehouseManagementSystem/alerts.h"
#include "../WarehouseManagementSystem/inventory.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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

    static int run_with_captured_stderr(
        const Product* head,
        int threshold,
        const char* path,
        char* error_output,
        size_t error_output_size
    )
    {
        FILE* temporary_error = nullptr;
        int saved_stderr;
        int result;
        size_t error_length;

        Assert::AreEqual(0, tmpfile_s(&temporary_error));
        Assert::IsNotNull(temporary_error);

        saved_stderr = _dup(_fileno(stderr));
        Assert::IsTrue(saved_stderr >= 0);
        Assert::AreEqual(
            0,
            _dup2(_fileno(temporary_error), _fileno(stderr))
        );

        clearerr(stderr);
        result = check_low_stock(head, threshold, path);
        std::fflush(stderr);
        std::rewind(temporary_error);

        error_length = std::fread(
            error_output,
            1U,
            error_output_size - 1U,
            temporary_error
        );
        error_output[error_length] = '\0';

        Assert::AreEqual(0, _dup2(saved_stderr, _fileno(stderr)));
        _close(saved_stderr);
        std::fclose(temporary_error);
        clearerr(stderr);

        return result;
    }

    static void assert_product_unchanged(
        const Product* product,
        const Product& before
    )
    {
        Assert::AreEqual(before.id, product->id);
        Assert::IsTrue(std::strcmp(before.name, product->name) == 0);
        Assert::AreEqual(before.quantity, product->quantity);
        Assert::IsTrue(std::strcmp(before.location, product->location) == 0);
        Assert::IsTrue(before.next == product->next);
    }

    TEST_CLASS(AlertsFunctionalTests)
    {
    public:
        TEST_METHOD(ALT_F_001_LowStockDetection)
        {
            const char* report_path = "alerts_below_threshold.txt";
            Product* head = nullptr;

            Assert::AreEqual(0, inventory_add_product(
                &head, 1001U, "Hammer", 5, "A-01"));
            Assert::AreEqual(0, inventory_add_product(
                &head, 1002U, "Safety Gloves", 10, "B-02"));
            Assert::AreEqual(0, inventory_add_product(
                &head, 1003U, "Drill Press", 25, "C-03"));

            Product* second = head->next;
            Product* third = second->next;
            const Product first_before = *head;
            const Product second_before = *second;
            const Product third_before = *third;

            Assert::AreEqual(1, check_low_stock(head, 10, report_path));

            const std::string report = read_file(report_path);
            Assert::IsTrue(report.find("Threshold: 10") != std::string::npos);
            Assert::IsTrue(report.find("1001") != std::string::npos);
            Assert::IsTrue(report.find("Hammer") != std::string::npos);
            Assert::IsTrue(report.find("1002") == std::string::npos);
            Assert::IsTrue(report.find("1003") == std::string::npos);

            assert_product_unchanged(head, first_before);
            assert_product_unchanged(second, second_before);
            assert_product_unchanged(third, third_before);

            inventory_free_all(&head);
            std::remove(report_path);
        }

        TEST_METHOD(ALT_F_002_NoMatchesAndBoundary)
        {
            const char* report_path = "alerts_no_matches.txt";
            Product* head = nullptr;

            Assert::AreEqual(0, inventory_add_product(
                &head, 1001U, "Safety Gloves", 10, "A-01"));
            Assert::AreEqual(0, inventory_add_product(
                &head, 1002U, "Drill Press", 25, "B-02"));

            Assert::AreEqual(0, check_low_stock(head, 10, report_path));
            Assert::IsTrue(read_file(report_path).find(
                "No products are below the configured threshold."
            ) != std::string::npos);

            inventory_free_all(&head);
            std::remove(report_path);
        }

        TEST_METHOD(ALT_F_003_InvalidInputs)
        {
            const char* report_path = "alerts_invalid_inputs.txt";
            Product* head = nullptr;
            char errors[256] = "";

            Assert::AreEqual(0, inventory_add_product(
                &head, 1001U, "Hammer", 5, "A-01"));
            write_file(report_path, "sentinel-content\n");
            Assert::AreEqual(-1, run_with_captured_stderr(
                head, 0, report_path, errors, sizeof(errors)));
            Assert::IsTrue(std::strstr(
                errors, "Invalid stock threshold."
            ) != nullptr);
            Assert::IsTrue(read_file(report_path).find(
                "sentinel-content"
            ) != std::string::npos);

            Assert::AreEqual(-1, run_with_captured_stderr(
                head, 10, nullptr, errors, sizeof(errors)));
            Assert::IsTrue(std::strstr(
                errors, "Invalid alert file path."
            ) != nullptr);

            Assert::AreEqual(-1, run_with_captured_stderr(
                head, 10, ".", errors, sizeof(errors)));
            Assert::IsTrue(std::strstr(
                errors, "Unable to open alert report file."
            ) != nullptr);

            inventory_free_all(&head);
            std::remove(report_path);
        }

        TEST_METHOD(ALT_F_004_OverwriteAndCleanup)
        {
            const char* report_path = "alerts_overwrite.txt";
            Product* head = nullptr;

            Assert::AreEqual(0, inventory_add_product(
                &head, 1001U, "Hammer", 5, "A-01"));
            Assert::AreEqual(0, inventory_add_product(
                &head, 1002U, "Safety Gloves", 10, "B-02"));
            Assert::AreEqual(0, inventory_add_product(
                &head, 1003U, "Drill Press", 25, "C-03"));

            Assert::AreEqual(2, check_low_stock(head, 11, report_path));
            Assert::AreEqual(1, check_low_stock(head, 6, report_path));

            const std::string report = read_file(report_path);
            Assert::IsTrue(report.find("Threshold: 6") != std::string::npos);
            Assert::IsTrue(report.find("Hammer") != std::string::npos);
            Assert::IsTrue(report.find("Safety Gloves") == std::string::npos);
            Assert::IsTrue(report.find("Drill Press") == std::string::npos);

            inventory_free_all(&head);
            Assert::IsNull(head);

            Assert::AreEqual(0, check_low_stock(head, 6, report_path));
            Assert::IsTrue(read_file(report_path).find(
                "No products are below the configured threshold."
            ) != std::string::npos);
            Assert::IsTrue(read_file(report_path).find(
                "Product ID:"
            ) == std::string::npos);

            std::remove(report_path);
        }
    };
}
