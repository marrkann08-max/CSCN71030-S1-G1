#include "pch.h"
#include "CppUnitTest.h"

#include "../WarehouseManagementSystem/utilities.h"

#include <cstring>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UtilitiesTests
{
    TEST_CLASS(UtilitiesFunctionalTests)
    {
    public:

        TEST_METHOD(UTL_F_001_ValidStrings)
        {
            char surrounded[] = "  Hammer  ";
            char internal_space[] = "Safety Gloves";
            char single_character[] = "A";
            char overlength[65];

            for (size_t index = 0; index < 64; ++index)
            {
                overlength[index] = 'X';
            }

            overlength[64] = '\0';

            Assert::AreEqual(
                0,
                util_validate_string(surrounded, 63)
            );
            Assert::IsTrue(std::strcmp(surrounded, "Hammer") == 0);

            Assert::AreEqual(
                0,
                util_validate_string(internal_space, 63)
            );
            Assert::IsTrue(
                std::strcmp(internal_space, "Safety Gloves") == 0
            );

            Assert::AreEqual(
                0,
                util_validate_string(single_character, 63)
            );
            Assert::IsTrue(
                std::strcmp(single_character, "A") == 0
            );

            Assert::AreEqual(
                0,
                util_validate_string(overlength, 63)
            );
            Assert::IsTrue(std::strlen(overlength) == 63);
            Assert::IsTrue(overlength[63] == '\0');
        }

        TEST_METHOD(UTL_F_002_InvalidStrings)
        {
            char empty[] = "";
            char whitespace_only[] = "     ";
            char control_character[] = "Hammer\tLocation";
            char zero_maximum[] = "Hammer";

            Assert::AreEqual(
                -1,
                util_validate_string(nullptr, 63)
            );
            Assert::AreEqual(
                -1,
                util_validate_string(empty, 63)
            );
            Assert::AreEqual(
                -1,
                util_validate_string(whitespace_only, 63)
            );
            Assert::AreEqual(
                -1,
                util_validate_string(control_character, 63)
            );
            Assert::AreEqual(
                -1,
                util_validate_string(zero_maximum, 0)
            );
        }

        TEST_METHOD(UTL_F_003_SignedConversion)
        {
            int output = 99;

            Assert::AreEqual(
                0,
                util_string_to_int("0", -10, 10, &output)
            );
            Assert::AreEqual(0, output);

            Assert::AreEqual(
                0,
                util_string_to_int("-10", -10, 10, &output)
            );
            Assert::AreEqual(-10, output);

            Assert::AreEqual(
                0,
                util_string_to_int("10", -10, 10, &output)
            );
            Assert::AreEqual(10, output);

            const char* invalid_inputs[] =
            {
                "-11",
                "11",
                "5abc",
                "",
                "2147483648"
            };

            for (const char* input : invalid_inputs)
            {
                output = 77;

                Assert::AreEqual(
                    -1,
                    util_string_to_int(input, -10, 10, &output)
                );
                Assert::AreEqual(77, output);
            }

            Assert::AreEqual(
                -1,
                util_string_to_int(nullptr, -10, 10, &output)
            );
            Assert::AreEqual(
                -1,
                util_string_to_int("5", -10, 10, nullptr)
            );
        }

        TEST_METHOD(UTL_F_004_UnsignedConversion)
        {
            unsigned int output = 999U;

            Assert::AreEqual(
                0,
                util_string_to_uint("0", 0U, 100U, &output)
            );
            Assert::AreEqual(0U, output);

            Assert::AreEqual(
                0,
                util_string_to_uint("100", 0U, 100U, &output)
            );
            Assert::AreEqual(100U, output);

            const char* invalid_inputs[] =
            {
                "101",
                "-1",
                "25abc",
                "4294967296"
            };

            for (const char* input : invalid_inputs)
            {
                output = 77U;

                Assert::AreEqual(
                    -1,
                    util_string_to_uint(input, 0U, 100U, &output)
                );
                Assert::AreEqual(77U, output);
            }

            Assert::AreEqual(
                -1,
                util_string_to_uint(nullptr, 0U, 100U, &output)
            );
            Assert::AreEqual(
                -1,
                util_string_to_uint("5", 0U, 100U, nullptr)
            );
        }

        TEST_METHOD(UTL_NullPointerCheck)
        {
            int value = 5;

            Assert::AreEqual(0, util_check_null(&value));
            Assert::AreEqual(-1, util_check_null(nullptr));
        }
    };
}