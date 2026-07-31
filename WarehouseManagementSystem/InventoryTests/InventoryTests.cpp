#include "pch.h"
#include "CppUnitTest.h"

#include "../WarehouseManagementSystem/inventory.h"

#include <cstring>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InventoryTests
{
    static size_t count_products(const Product* head)
    {
        size_t count = 0;

        while (head != nullptr)
        {
            ++count;
            head = head->next;
        }

        return count;
    }

    TEST_CLASS(InventoryFunctionalTests)
    {
    public:

        TEST_METHOD(INV_F_001_AddAndRetrieve)
        {
            Product* head = nullptr;

            Assert::AreEqual(
                0,
                inventory_add_product(
                    &head,
                    1001U,
                    " Hammer ",
                    5,
                    " A-01 "
                )
            );

            Assert::IsNotNull(head);
            Assert::AreEqual(1001U, head->id);
            Assert::IsTrue(std::strcmp(head->name, "Hammer") == 0);
            Assert::AreEqual(5, head->quantity);
            Assert::IsTrue(std::strcmp(head->location, "A-01") == 0);
            Assert::IsNull(head->next);
            Assert::AreEqual(static_cast<size_t>(1), count_products(head));

            Product* retrieved =
                inventory_get_product(head, 1001U);

            Assert::IsTrue(retrieved == head);

            inventory_free_all(&head);
        }

        TEST_METHOD(INV_F_002_RejectInvalidAndTruncate)
        {
            Product* head = nullptr;
            char long_name[81];
            char long_location[41];

            std::memset(long_name, 'N', 80);
            long_name[80] = '\0';

            std::memset(long_location, 'L', 40);
            long_location[40] = '\0';

            Assert::AreEqual(
                0,
                inventory_add_product(
                    &head,
                    1001U,
                    "Hammer",
                    5,
                    "A-01"
                )
            );

            Assert::AreEqual(
                -1,
                inventory_add_product(
                    &head,
                    1001U,
                    "Duplicate",
                    1,
                    "A-02"
                )
            );

            Assert::AreEqual(
                -1,
                inventory_add_product(
                    &head,
                    0U,
                    "Invalid ID",
                    1,
                    "A-02"
                )
            );

            Assert::AreEqual(
                -1,
                inventory_add_product(
                    &head,
                    1002U,
                    "Invalid Quantity",
                    -1,
                    "A-02"
                )
            );

            Assert::AreEqual(
                -1,
                inventory_add_product(
                    &head,
                    1002U,
                    "",
                    1,
                    "A-02"
                )
            );

            Assert::AreEqual(
                -1,
                inventory_add_product(
                    &head,
                    1002U,
                    "Other Product",
                    1,
                    "   "
                )
            );

            Assert::AreEqual(
                static_cast<size_t>(1),
                count_products(head)
            );

            Assert::AreEqual(
                0,
                inventory_add_product(
                    &head,
                    1002U,
                    long_name,
                    1,
                    long_location
                )
            );

            Product* product =
                inventory_get_product(head, 1002U);

            Assert::IsNotNull(product);
            Assert::AreEqual(
                static_cast<size_t>(63),
                std::strlen(product->name)
            );
            Assert::AreEqual(
                static_cast<size_t>(31),
                std::strlen(product->location)
            );
            Assert::AreEqual('\0', product->name[63]);
            Assert::AreEqual('\0', product->location[31]);
            Assert::AreEqual(
                static_cast<size_t>(2),
                count_products(head)
            );

            Product* original =
                inventory_get_product(head, 1001U);

            Assert::IsNotNull(original);
            Assert::IsTrue(
                std::strcmp(original->name, "Hammer") == 0
            );
            Assert::AreEqual(5, original->quantity);

            inventory_free_all(&head);
        }

        TEST_METHOD(INV_F_003_UpdateQuantity)
        {
            Product* head = nullptr;

            Assert::AreEqual(
                0,
                inventory_add_product(
                    &head,
                    1001U,
                    "Hammer",
                    5,
                    "A-01"
                )
            );

            Assert::AreEqual(
                0,
                inventory_update_quantity(head, 1001U, 12)
            );
            Assert::AreEqual(12, head->quantity);

            Assert::AreEqual(
                -1,
                inventory_update_quantity(head, 9999U, 20)
            );
            Assert::AreEqual(12, head->quantity);

            Assert::AreEqual(
                -1,
                inventory_update_quantity(head, 1001U, -1)
            );
            Assert::AreEqual(12, head->quantity);

            inventory_free_all(&head);
        }

        TEST_METHOD(INV_F_004_DeleteProducts)
        {
            Product* head = nullptr;

            inventory_add_product(
                &head, 1001U, "Hammer", 5, "A-01"
            );
            inventory_add_product(
                &head, 1002U, "Drill", 3, "A-02"
            );
            inventory_add_product(
                &head, 1003U, "Gloves", 10, "A-03"
            );

            Assert::AreEqual(
                -1,
                inventory_delete_product(&head, 9999U)
            );
            Assert::AreEqual(
                static_cast<size_t>(3),
                count_products(head)
            );

            Assert::AreEqual(
                0,
                inventory_delete_product(&head, 1002U)
            );
            Assert::AreEqual(
                static_cast<size_t>(2),
                count_products(head)
            );
            Assert::AreEqual(1001U, head->id);
            Assert::IsNotNull(head->next);
            Assert::AreEqual(1003U, head->next->id);

            Assert::AreEqual(
                0,
                inventory_delete_product(&head, 1001U)
            );
            Assert::AreEqual(1003U, head->id);

            Assert::AreEqual(
                0,
                inventory_delete_product(&head, 1003U)
            );
            Assert::IsNull(head);
        }

        TEST_METHOD(INV_F_005_FreeAllProducts)
        {
            Product* head = nullptr;

            inventory_add_product(
                &head, 1001U, "Hammer", 5, "A-01"
            );
            inventory_add_product(
                &head, 1002U, "Drill", 3, "A-02"
            );
            inventory_add_product(
                &head, 1003U, "Gloves", 10, "A-03"
            );

            Assert::AreEqual(
                static_cast<size_t>(3),
                count_products(head)
            );

            inventory_free_all(&head);

            Assert::IsNull(head);

            // A second cleanup must also be safe.
            inventory_free_all(&head);
            Assert::IsNull(head);

            // A NULL Product** must not crash.
            inventory_free_all(nullptr);
        }
    };
}