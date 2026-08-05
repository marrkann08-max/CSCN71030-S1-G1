#include "pch.h"
#include "CppUnitTest.h"

#include <cstring>

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

            char username[] = "  admin  ";
            char password[] = "  Admin123  ";

            Assert::AreEqual(0, authenticate(username, password));
            Assert::IsTrue(std::strcmp(username, "  admin  ") == 0);
            Assert::IsTrue(std::strcmp(password, "  Admin123  ") == 0);
        }

        TEST_METHOD(AUT_F_002_InvalidAndCaseSensitiveCredentials)
        {
            Assert::AreEqual(-1, authenticate("unknown", "Admin123"));
            Assert::AreEqual(-1, authenticate("admin", "wrong"));
            Assert::AreEqual(-1, authenticate("Admin", "Admin123"));
            Assert::AreEqual(-1, authenticate("admin", "admin123"));
        }

        TEST_METHOD(AUT_F_003_InvalidCredentialInputs)
        {
            char overlength[AUTH_FIELD_SIZE + 1U];

            std::memset(overlength, 'x', AUTH_FIELD_SIZE);
            overlength[AUTH_FIELD_SIZE] = '\0';

            Assert::AreEqual(-1, authenticate(nullptr, "Admin123"));
            Assert::AreEqual(-1, authenticate("admin", nullptr));
            Assert::AreEqual(-1, authenticate("", "Admin123"));
            Assert::AreEqual(-1, authenticate("admin", ""));
            Assert::AreEqual(-1, authenticate("   ", "Admin123"));
            Assert::AreEqual(-1, authenticate("admin", "\t \r\n"));
            Assert::AreEqual(-1, authenticate(overlength, "Admin123"));
            Assert::AreEqual(-1, authenticate("admin", overlength));
        }
    };
}
