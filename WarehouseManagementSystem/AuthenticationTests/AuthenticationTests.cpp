#include "pch.h"
#include "CppUnitTest.h"

#include <cstring>
#include <cstdio>
#include <io.h>

extern "C"
{
#include "../WarehouseManagementSystem/authentication.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AuthenticationTests
{
    static int run_login_with_input(
        const char* input,
        char* output,
        size_t output_size
    )
    {
        FILE* temporary_input = nullptr;
        int saved_stdin;
        int result;

        Assert::AreEqual(0, tmpfile_s(&temporary_input));
        Assert::IsNotNull(temporary_input);

        std::fputs(input, temporary_input);
        std::rewind(temporary_input);

        saved_stdin = _dup(_fileno(stdin));
        Assert::IsTrue(saved_stdin >= 0);
        Assert::AreEqual(
            0,
            _dup2(_fileno(temporary_input), _fileno(stdin))
        );

        clearerr(stdin);
        result = authentication_login(output, output_size);

        Assert::AreEqual(0, _dup2(saved_stdin, _fileno(stdin)));
        _close(saved_stdin);
        std::fclose(temporary_input);
        clearerr(stdin);

        return result;
    }

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

        TEST_METHOD(AUT_F_004_LoginAttemptHandling)
        {
            char output[AUTH_FIELD_SIZE] = "UNCHANGED";
            char small_output[3] = "NO";

            Assert::AreEqual(
                -1,
                authentication_login(nullptr, sizeof(output))
            );
            Assert::AreEqual(-1, authentication_login(output, 0U));
            Assert::IsTrue(std::strcmp(output, "UNCHANGED") == 0);

            Assert::AreEqual(
                -1,
                run_login_with_input(
                    "admin\nAdmin123\n",
                    small_output,
                    sizeof(small_output)
                )
            );
            Assert::IsTrue(std::strcmp(small_output, "NO") == 0);

            Assert::AreEqual(
                -1,
                run_login_with_input(
                    "bad\nbad\nbad\nbad\nbad\nbad\n",
                    output,
                    sizeof(output)
                )
            );
            Assert::IsTrue(std::strcmp(output, "UNCHANGED") == 0);

            Assert::AreEqual(
                0,
                run_login_with_input(
                    "bad\nbad\nunknown\nwrong\n  admin  \n  Admin123  \n",
                    output,
                    sizeof(output)
                )
            );
            Assert::IsTrue(std::strcmp(output, "admin") == 0);
        }
    };
}
