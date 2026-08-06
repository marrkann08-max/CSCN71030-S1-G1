#include "pch.h"
#include "CppUnitTest.h"

#include <climits>
#include <cstdio>
#include <cstring>
#include <string>

extern "C" {
#include "../WarehouseManagementSystem/orders.h"
#include "../WarehouseManagementSystem/Logger.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace orderstests
{
    static const char* integration_log_path = "orders_integration_test.log";
    static int logger_result = 0;
    static int logger_calls = 0;
    static int logger_last_approved = -1;
    static char logger_last_type = '\0';
    static std::string logger_last_product;

    static int test_logger(
        unsigned int,
        const char* product_name,
        int,
        char transaction_type,
        int approved
    )
    {
        ++logger_calls;
        logger_last_product = product_name;
        logger_last_type = transaction_type;
        logger_last_approved = approved;
        return logger_result;
    }

    TEST_CLASS(OrdersTests)
    {
    public:
        TEST_METHOD_INITIALIZE(Initialize)
        {
            logger_result = 0;
            logger_calls = 0;
            logger_last_approved = -1;
            logger_last_type = '\0';
            logger_last_product.clear();
            order_set_logger(test_logger);
        }

        TEST_METHOD_CLEANUP(Cleanup)
        {
            logger_shutdown();
            (void)std::remove(integration_log_path);
            order_set_logger(nullptr);
        }

        // OR-F-001: create valid orders and reject every invalid input.
        TEST_METHOD(OR_F_001_CreateAndValidate)
        {
            Order* receive = order_create(1001U, 3, ORDER_RECEIVE);
            Assert::IsNotNull(receive);
            Assert::AreEqual(1001U, receive->productID);
            Assert::AreEqual(3, receive->quantity);
            Assert::AreEqual((int)ORDER_RECEIVE, (int)receive->orderType);
            Assert::AreEqual((int)ORDER_NOT_STARTED, (int)receive->status);
            order_free(receive);

            Assert::IsNull(order_create(0U, 3, ORDER_RECEIVE));
            Assert::IsNull(order_create(1001U, 0, ORDER_RECEIVE));
            Assert::IsNull(order_create(1001U, -1, ORDER_RECEIVE));
            Assert::IsNull(order_create(1001U, 3, 'X'));
            Assert::AreEqual(0, logger_calls);
            order_free(nullptr);
        }

        // OR-F-002: process receive/dispatch and preserve non-quantity fields.
        TEST_METHOD(OR_F_002_ProcessStock)
        {
            Product* head = nullptr;
            Assert::AreEqual(0, inventory_add_product(&head, 1001U, "Hammer", 10, "A-01"));
            Product* original_next = head->next;

            Order* receive = order_create(1001U, 5, ORDER_RECEIVE);
            Assert::AreEqual(ORDER_SUCCESS, order_process(head, receive));
            Assert::AreEqual(15, head->quantity);
            Assert::AreEqual((int)ORDER_COMPLETED, (int)receive->status);
            Assert::AreEqual(0, std::strcmp("Hammer", head->name));
            Assert::AreEqual(0, std::strcmp("A-01", head->location));
            Assert::IsTrue(head->next == original_next);
            Assert::AreEqual((int)ORDER_RECEIVE, (int)logger_last_type);
            Assert::AreEqual(1, logger_last_approved);
            Assert::AreEqual(std::string("Hammer"), logger_last_product);
            order_free(receive);

            Order* dispatch = order_create(1001U, 4, ORDER_DISPATCH);
            Assert::AreEqual(ORDER_SUCCESS, order_process(head, dispatch));
            Assert::AreEqual(11, head->quantity);
            Assert::AreEqual((int)ORDER_COMPLETED, (int)dispatch->status);
            Assert::AreEqual((int)ORDER_DISPATCH, (int)logger_last_type);
            Assert::AreEqual(1, logger_last_approved);
            order_free(dispatch);
            inventory_free_all(&head);
            Assert::IsNull(head);
        }

        // OR-F-003: reject unsafe operations and preserve inventory atomically.
        TEST_METHOD(OR_F_003_RejectionsAndAtomicity)
        {
            Product* head = nullptr;
            Assert::AreEqual(0, inventory_add_product(&head, 1001U, "Hammer", 5, "A-01"));

            Order* insufficient = order_create(1001U, 6, ORDER_DISPATCH);
            Assert::AreEqual(ORDER_FAILURE, order_process(head, insufficient));
            Assert::AreEqual(5, head->quantity);
            Assert::AreEqual((int)ORDER_REJECTED, (int)insufficient->status);
            Assert::AreEqual(0, logger_last_approved);
            order_free(insufficient);

            Order* missing = order_create(9999U, 1, ORDER_RECEIVE);
            Assert::AreEqual(ORDER_FAILURE, order_process(head, missing));
            Assert::AreEqual(5, head->quantity);
            order_free(missing);

            head->quantity = INT_MAX;
            Order* overflow = order_create(1001U, 1, ORDER_RECEIVE);
            Assert::AreEqual(ORDER_FAILURE, order_process(head, overflow));
            Assert::AreEqual(INT_MAX, head->quantity);
            order_free(overflow);

            head->quantity = 5;
            logger_result = -1;
            Order* logging_failure = order_create(1001U, 3, ORDER_RECEIVE);
            Assert::AreEqual(ORDER_FAILURE, order_process(head, logging_failure));
            Assert::AreEqual(5, head->quantity);
            Assert::AreEqual((int)ORDER_REJECTED, (int)logging_failure->status);
            order_free(logging_failure);

            Assert::AreEqual(ORDER_FAILURE, order_process(nullptr, nullptr));
            Assert::AreEqual(ORDER_FAILURE, processReceive(nullptr, 1001U, 1));
            Assert::AreEqual(ORDER_FAILURE, processDispatch(head, 1001U, 0));
            inventory_free_all(&head);
        }

        // OR-F-004: simulation is reproducible and uses the full shared list.
        TEST_METHOD(OR_F_004_DeterministicSimulation)
        {
            Product* first = nullptr;
            Product* second = nullptr;
            Assert::AreEqual(0, inventory_add_product(&first, 1001U, "Hammer", 20, "A-01"));
            Assert::AreEqual(0, inventory_add_product(&first, 1002U, "Saw", 20, "A-02"));
            Assert::AreEqual(0, inventory_add_product(&second, 1001U, "Hammer", 20, "A-01"));
            Assert::AreEqual(0, inventory_add_product(&second, 1002U, "Saw", 20, "A-02"));

            Assert::AreEqual(order_simulate(first, 2026U), order_simulate(second, 2026U));
            Assert::AreEqual(first->quantity, second->quantity);
            Assert::AreEqual(first->next->quantity, second->next->quantity);
            Assert::IsTrue(first->quantity != 20 || first->next->quantity != 20);
            inventory_free_all(&first);
            inventory_free_all(&second);
        }

        // OR-I-001: Orders integrates with Inventory, Utilities, and Logger contract.
        TEST_METHOD(OR_I_001_SharedModuleIntegration)
        {
            Product* head = nullptr;
            FILE* log_file = nullptr;
            char log_text[2048] = { 0 };
            size_t bytes_read;

            order_set_logger(logger_write_transaction);
            (void)std::remove(integration_log_path);
            Assert::AreEqual(0, logger_initialize(integration_log_path, "orders_test"));
            Assert::AreEqual(0, inventory_add_product(&head, 77U, "  Bolts  ", 8, " B-02 "));
            Assert::AreEqual(0, std::strcmp("Bolts", head->name));
            Assert::AreEqual(0, std::strcmp("B-02", head->location));

            Order* order = order_create(77U, 2, ORDER_DISPATCH);
            Assert::IsNotNull(order);
            Assert::AreEqual(ORDER_SUCCESS, order_process(head, order));
            Assert::AreEqual(6, inventory_get_product(head, 77U)->quantity);
            order_free(order);
            inventory_free_all(&head);
            logger_shutdown();

            Assert::AreEqual(0, fopen_s(&log_file, integration_log_path, "r"));
            Assert::IsNotNull(log_file);
            bytes_read = fread(log_text, 1U, sizeof(log_text) - 1U, log_file);
            log_text[bytes_read] = '\0';
            fclose(log_file);
            Assert::IsTrue(std::strstr(log_text, "Product ID: 77") != nullptr);
            Assert::IsTrue(std::strstr(log_text, "Product: Bolts") != nullptr);
            Assert::IsTrue(std::strstr(log_text, "Type: Dispatch") != nullptr);
            Assert::IsTrue(std::strstr(log_text, "Status: Approved") != nullptr);
        }
    };
}
