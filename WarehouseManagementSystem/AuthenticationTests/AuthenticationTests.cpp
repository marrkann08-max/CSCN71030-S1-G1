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
        size_t output_size,
        char* error_output,
        size_t error_output_size,
        char* remaining_input,
        size_t remaining_input_size
    )
    {
        FILE* temporary_input = nullptr;
        FILE* temporary_error = nullptr;
        int saved_stdin;
        int saved_stderr;
        int result;
        size_t error_length;

        Assert::AreEqual(0, tmpfile_s(&temporary_input));
        Assert::IsNotNull(temporary_input);
        Assert::AreEqual(0, tmpfile_s(&temporary_error));
        Assert::IsNotNull(temporary_error);

        std::fputs(input, temporary_input);
        std::rewind(temporary_input);

        saved_stdin = _dup(_fileno(stdin));
        saved_stderr = _dup(_fileno(stderr));
        Assert::IsTrue(saved_stdin >= 0);
        Assert::IsTrue(saved_stderr >= 0);
        Assert::AreEqual(
            0,
            _dup2(_fileno(temporary_input), _fileno(stdin))
        );
        Assert::AreEqual(
            0,
            _dup2(_fileno(temporary_error), _fileno(stderr))
        );

        Assert::AreEqual(0, setvbuf(stdin, nullptr, _IONBF, 0));
        clearerr(stdin);
        clearerr(stderr);
        result = authentication_login(output, output_size);

        if (remaining_input != nullptr && remaining_input_size > 0U)
        {
            if (std::fgets(
                    remaining_input,
                    (int)remaining_input_size,
                    stdin
                ) == nullptr)
            {
                remaining_input[0] = '\0';
            }
        }

        std::fflush(stderr);
        std::rewind(temporary_error);
        error_length = std::fread(
            error_output,
            1U,
            error_output_size - 1U,
            temporary_error
        );
        error_output[error_length] = '\0';

        Assert::AreEqual(0, _dup2(saved_stdin, _fileno(stdin)));
        Assert::AreEqual(0, _dup2(saved_stderr, _fileno(stderr)));
        _close(saved_stdin);
        _close(saved_stderr);
        std::fclose(temporary_input);
        std::fclose(temporary_error);
        clearerr(stdin);
        clearerr(stderr);

        return result;
    }

    static size_t count_occurrences(
        const char* text,
        const char* expected
    )
    {
        size_t count = 0U;
        const char* current = text;

        while ((current = std::strstr(current, expected)) != nullptr)
        {
            ++count;
            current += std::strlen(expected);
        }

        return count;
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
            char errors[512] = "";
            char remaining[AUTH_FIELD_SIZE] = "";
            const char* invalid_message =
                "Invalid username or password.\n";
            const char* denied_message =
                "Access denied after three failed attempts.\n";

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
                    sizeof(small_output),
                    errors,
                    sizeof(errors),
                    remaining,
                    sizeof(remaining)
                )
            );
            Assert::IsTrue(std::strcmp(small_output, "NO") == 0);

            Assert::AreEqual(
                0,
                run_login_with_input(
                    "bad\nbad\n  admin  \n  Admin123  \nunused\nUnused123\n",
                    output,
                    sizeof(output),
                    errors,
                    sizeof(errors),
                    remaining,
                    sizeof(remaining)
                )
            );
            Assert::IsTrue(std::strcmp(output, "admin") == 0);
            Assert::AreEqual<size_t>(
                1U,
                count_occurrences(errors, invalid_message)
            );
            Assert::IsNull(std::strstr(errors, denied_message));
            Assert::IsTrue(std::strcmp(remaining, "unused\n") == 0);

            strcpy_s(output, "UNCHANGED");

            Assert::AreEqual(
                -1,
                run_login_with_input(
                    "bad\nbad\nunknown\nwrong\ninvalid\ncredentials\n"
                    "admin\nAdmin123\n",
                    output,
                    sizeof(output),
                    errors,
                    sizeof(errors),
                    remaining,
                    sizeof(remaining)
                )
            );
            Assert::IsTrue(std::strcmp(output, "UNCHANGED") == 0);
            Assert::AreEqual<size_t>(
                3U,
                count_occurrences(errors, invalid_message)
            );
            Assert::AreEqual<size_t>(
                1U,
                count_occurrences(errors, denied_message)
            );
            Assert::IsTrue(std::strcmp(remaining, "admin\n") == 0);
        }
    };
}
