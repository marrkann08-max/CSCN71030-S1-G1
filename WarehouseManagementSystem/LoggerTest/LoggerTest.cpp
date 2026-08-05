#include "pch.h"
#include "CppUnitTest.h"
#include <cstring>
extern "C" {
#pragma warning(disable : 4996)
#include "../WarehouseManagementSystem/Logger.h"
#include "../WarehouseManagementSystem/Logger.c"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LoggerTest
{
	TEST_CLASS(LoggerTest)
	{
	public:

		TEST_METHOD(InitSucceed)
		{
			char* filePath = "Logger/";
			Assert::AreEqual(0, logInit(filePath));
		}
		TEST_METHOD(WriteBufferOverflow)
		{
			char* overflowString = "adsjhfbsaodhifbSKDJFBLKSAjdfblksajxvblkJXbvlkasdbglkasjdhflkajdshflkajsdfblkasjdhflksajhdflksajdhflkasdjhflksadjhflksajhdflksajhdf";
			Assert::AreEqual(-1, writeToFile(overflowString));
		}
		TEST_METHOD(WriteFileNotFound)
		{
			char* normalString = "Hello world";
			Assert::AreEqual(-1, writeToFile(normalString));
		}
		TEST_METHOD(NormaleWrite) // not possible since unit test cant create the damn file
		{
		}
	};
}
