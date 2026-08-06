//--------------------------------------------------------------------------
//		Configtests.cpp - Unit tests for the Config module of the WMS
//						Author: Ivan Immanuel Shaji
//--------------------------------------------------------------------------

#include "pch.h"
#include "CppUnitTest.h"

extern "C"
{
#include "../WarehouseManagementSystem/config.h"
}

#include <climits>
#include <cstdio>
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ConfigTests
{
	/*
	 * Author: Ivan Immanuel Shaji
	 * Input: A file path and ASCII configuration text.
	 * Output: Creates or replaces the specified test file.
	 * Purpose: Prepare controlled Config file input for functional tests.
	 */
	static void write_text_file(const char* path, const char* text)
	{
		std::ofstream output(path, std::ios::out | std::ios::trunc);

		Assert::IsTrue(output.is_open());

		output << text;
		output.close();

		Assert::IsTrue(output.good());
	}

	TEST_CLASS(ConfigFunctionalTests)
	{
	public:

		/*
		 * Author: Ivan Immanuel Shaji
		 * Input: No configuration file and a startup argument array.
		 * Output: Verifies defaults and null-output rejection.
		 * Purpose: Test safe Config initialization when no file is available.
		 */
		TEST_METHOD(CFG_F_001_DefaultsAndNullOutput)
		{
			std::remove(CONFIG_FILE_NAME);

			char executable[] = "wms.exe";
			char* argv[] = { executable };
			Config config = { 99, 99U };

			Assert::AreEqual(0, load_config(1, argv, &config));
			Assert::AreEqual(DEFAULT_THRESHOLD, config.threshold);
			Assert::AreEqual(DEFAULT_SEED, config.seed);
			Assert::AreEqual(-1, load_config(1, argv, nullptr));

			print_config(&config);
			print_config(nullptr);
		}

		/*
		 * Author: Ivan Immanuel Shaji
		 * Input: Valid, invalid, and unavailable ASCII configuration files.
		 * Output: Verifies file values and preservation of earlier values.
		 * Purpose: Test Config ASCII file reading and validation.
		 */
		TEST_METHOD(CFG_F_002_FileReading)
		{
			const char* path = "config_test.txt";
			Config config = { DEFAULT_THRESHOLD, DEFAULT_SEED };

			write_text_file(path, "threshold=25\nseed=2026\n");

			Assert::AreEqual(0, read_config_file(path, &config));
			Assert::AreEqual(25, config.threshold);
			Assert::AreEqual(2026U, config.seed);

			write_text_file(path, "threshold=0\nseed=invalid\nunknown=7\n");

			Assert::AreEqual(0, read_config_file(path, &config));
			Assert::AreEqual(25, config.threshold);
			Assert::AreEqual(2026U, config.seed);

			Assert::AreEqual(0, std::remove(path));
			Assert::AreEqual(-1, read_config_file(path, &config));
		}

		/*
		 * Author: Ivan Immanuel Shaji
		 * Input: File settings followed by repeated valid CLI settings.
		 * Output: Verifies that the last valid CLI values are stored.
		 * Purpose: Test file/CLI precedence and duplicate-option handling.
		 */
		TEST_METHOD(CFG_F_003_CliPrecedence)
		{
			write_text_file(CONFIG_FILE_NAME, "threshold=20\nseed=200\n");

			char executable[] = "wms.exe";
			char threshold_one[] = "--threshold";
			char threshold_value_one[] = "30";
			char seed_one[] = "--seed";
			char seed_value_one[] = "300";
			char threshold_two[] = "--threshold";
			char threshold_value_two[] = "40";
			char seed_two[] = "--seed";
			char seed_value_two[] = "400";

			char* argv[] = { executable, threshold_one, threshold_value_one, seed_one, seed_value_one, threshold_two, threshold_value_two, seed_two, seed_value_two };

			Config config = { 0, 0U };

			Assert::AreEqual(0, load_config(9, argv, &config));
			Assert::AreEqual(40, config.threshold);
			Assert::AreEqual(400U, config.seed);
		}

		/*
		 * Author: Ivan Immanuel Shaji
		 * Input: Invalid, missing, unknown, and boundary CLI values.
		 * Output: Verifies rejection, preservation, and valid boundaries.
		 * Purpose: Test command-line validation through Utilities.
		 */
		TEST_METHOD(CFG_F_004_InvalidAndBoundaryCliValues)
		{
			Config config = { 20, 200U };

			char executable[] = "wms.exe";
			char threshold_flag[] = "--threshold";
			char zero[] = "0";
			char seed_flag[] = "--seed";
			char negative[] = "-1";
			char unknown[] = "--unknown";
			char unknown_value[] = "5";

			char* invalid_argv[] = { executable, threshold_flag, zero, seed_flag, negative, unknown, unknown_value };

			Assert::AreEqual(-1, apply_cli_overrides(7, invalid_argv, &config));
			Assert::AreEqual(20, config.threshold);
			Assert::AreEqual(200U, config.seed);

			char missing_seed[] = "--seed";
			char* missing_argv[] = { executable, missing_seed };

			Assert::AreEqual(-1, apply_cli_overrides(2, missing_argv, &config));
			Assert::AreEqual(200U, config.seed);

			char maximum_threshold[] = "2147483647";
			char maximum_seed[] = "4294967295";

			char* boundary_argv[] = { executable, threshold_flag, maximum_threshold, seed_flag, maximum_seed };

			Assert::AreEqual(0, apply_cli_overrides(5, boundary_argv, &config));
			Assert::AreEqual(INT_MAX, config.threshold);
			Assert::AreEqual(UINT_MAX, config.seed);
		}

		/*
		 * Author: Ivan Immanuel Shaji
		 * Input: Temporary files produced during a test.
		 * Output: Removes temporary Config test files.
		 * Purpose: Prevent tests from affecting later test executions.
		 */
		TEST_METHOD_CLEANUP(RemoveTemporaryFiles)
		{
			std::remove(CONFIG_FILE_NAME);
			std::remove("config_test.txt");
		}
	};
}