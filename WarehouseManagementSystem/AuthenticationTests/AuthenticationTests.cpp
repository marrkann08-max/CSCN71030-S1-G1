#include "pch.h"
#include "CppUnitTest.h"

extern "C"
{
#include "../WarehouseManagementSystem/authentication.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AuthenticationTests
{
    TEST_CLASS(AuthenticationFunctionalTests)
    {
    public:

        TEST_METHOD(AUT_F_001_ValidCredentials)
        {
            Assert::AreEqual(0, authenticate("admin", "Admin123"));
            Assert::AreEqual(0, authenticate("manager", "Manager123"));
            Assert::AreEqual(0, authenticate("warehouse", "Warehouse123"));
        }

        TEST_METHOD(AUT_F_002_InvalidAndCaseSensitiveCredentials)
        {
            Assert::AreEqual(-1, authenticate("unknown", "Admin123"));
            Assert::AreEqual(-1, authenticate("admin", "wrong"));
            Assert::AreEqual(-1, authenticate("Admin", "Admin123"));
            Assert::AreEqual(-1, authenticate("admin", "admin123"));
        }
    };
}
