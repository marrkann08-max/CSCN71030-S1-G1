#include "pch.h"
#include "CppUnitTest.h"

extern "C"
{
#include "../WarehouseManagementSystem/Logger.h"
}

#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LoggerTests
{
    static const char* TEST_LOG_PATH = "logger_test.log";

    static std::string read_log_file(void)
    {
        std::ifstream input(TEST_LOG_PATH, std::ios::in | std::ios::binary);

        Assert::IsTrue(input.is_open());

        return std::string(
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>()
        );
    }

    TEST_CLASS(LoggerFunctionalTests)
    {
    public:
        TEST_METHOD_INITIALIZE(ResetLogger)
        {
            logger_shutdown();
            std::remove(TEST_LOG_PATH);
        }

        TEST_METHOD_CLEANUP(CleanupLogger)
        {
            logger_shutdown();
            std::remove(TEST_LOG_PATH);
        }

        TEST_METHOD(LGR_F_001_InitializeAndShutdown)
        {
            Assert::AreEqual(-1, logger_initialize(nullptr, "warehouse_admin"));
            Assert::AreEqual(-1, logger_initialize(TEST_LOG_PATH, nullptr));
            Assert::AreEqual(-1, logger_initialize("", "warehouse_admin"));
            Assert::AreEqual(-1, logger_initialize(TEST_LOG_PATH, "   "));
            Assert::AreEqual(-1, logger_initialize("missing_folder/logger.log", "warehouse_admin"));

            Assert::AreEqual(0, logger_initialize(TEST_LOG_PATH, "warehouse_admin"));
            Assert::AreEqual(-1, logger_initialize(TEST_LOG_PATH, "second_user"));

            logger_shutdown();
            logger_shutdown();

            const std::string content = read_log_file();

            Assert::IsTrue(content.find("warehouse_admin: Program starting...") != std::string::npos);
            Assert::IsTrue(content.find("warehouse_admin: Program closing...") != std::string::npos);
        }

        TEST_METHOD(LGR_F_002_WriteValidatedText)
        {
            Assert::AreEqual(-1, logger_write_string("Before initialization"));
            Assert::AreEqual(0, logger_initialize(TEST_LOG_PATH, "operator_one"));
            Assert::AreEqual(-1, logger_write_string(nullptr));
            Assert::AreEqual(-1, logger_write_string(""));
            Assert::AreEqual(-1, logger_write_string("   "));
            Assert::AreEqual(0, logger_write_string("  Inventory opened  "));
            Assert::AreEqual(0, logger_write_string("Literal percent values: %s %d 100%"));

            logger_shutdown();

            const std::string content = read_log_file();

            Assert::IsTrue(content.find("operator_one: Inventory opened") != std::string::npos);
            Assert::IsTrue(content.find("Literal percent values: %s %d 100%") != std::string::npos);
        }

        TEST_METHOD(LGR_F_003_RecordTransactions)
        {
            Assert::AreEqual(0, logger_initialize(TEST_LOG_PATH, "shipping_user"));

            Assert::AreEqual(
                0,
                logger_write_transaction(1001U, "Hammer", 5, LOGGER_TRANSACTION_RECEIVE, 1)
            );
            Assert::AreEqual(
                0,
                logger_write_transaction(1002U, "Safety Gloves", 3, LOGGER_TRANSACTION_DISPATCH, 0)
            );

            logger_shutdown();

            const std::string content = read_log_file();

            Assert::IsTrue(content.find("shipping_user:") != std::string::npos);
            Assert::IsTrue(content.find("Product ID: 1001") != std::string::npos);
            Assert::IsTrue(content.find("Product: Hammer") != std::string::npos);
            Assert::IsTrue(content.find("Quantity: 5") != std::string::npos);
            Assert::IsTrue(content.find("Type: Receive") != std::string::npos);
            Assert::IsTrue(content.find("Status: Approved") != std::string::npos);
            Assert::IsTrue(content.find("Product ID: 1002") != std::string::npos);
            Assert::IsTrue(content.find("Product: Safety Gloves") != std::string::npos);
            Assert::IsTrue(content.find("Type: Dispatch") != std::string::npos);
            Assert::IsTrue(content.find("Status: Rejected") != std::string::npos);
        }

        TEST_METHOD(LGR_F_004_RejectInvalidTransactions)
        {
            Assert::AreEqual(0, logger_initialize(TEST_LOG_PATH, "inventory_user"));

            Assert::AreEqual(-1, logger_write_transaction(0U, "Hammer", 1, LOGGER_TRANSACTION_RECEIVE, 1));
            Assert::AreEqual(-1, logger_write_transaction(1001U, nullptr, 1, LOGGER_TRANSACTION_RECEIVE, 1));
            Assert::AreEqual(-1, logger_write_transaction(1001U, "", 1, LOGGER_TRANSACTION_RECEIVE, 1));
            Assert::AreEqual(-1, logger_write_transaction(1001U, "Hammer", 0, LOGGER_TRANSACTION_RECEIVE, 1));
            Assert::AreEqual(-1, logger_write_transaction(1001U, "Hammer", -1, LOGGER_TRANSACTION_RECEIVE, 1));
            Assert::AreEqual(-1, logger_write_transaction(1001U, "Hammer", 1, 'X', 1));
            Assert::AreEqual(-1, logger_write_transaction(1001U, "Hammer", 1, LOGGER_TRANSACTION_RECEIVE, 2));

            logger_shutdown();

            const std::string content = read_log_file();

            Assert::IsTrue(content.find("Transaction |") == std::string::npos);
        }

        TEST_METHOD(LGR_F_005_AppendAcrossSessions)
        {
            Assert::AreEqual(0, logger_initialize(TEST_LOG_PATH, "first_user"));
            Assert::AreEqual(0, logger_write_string("First session marker"));
            logger_shutdown();

            Assert::AreEqual(0, logger_initialize(TEST_LOG_PATH, "second_user"));
            Assert::AreEqual(0, logger_write_string("Second session marker"));
            logger_shutdown();

            const std::string content = read_log_file();

            Assert::IsTrue(content.find("first_user: First session marker") != std::string::npos);
            Assert::IsTrue(content.find("second_user: Second session marker") != std::string::npos);
        }
    };
}
