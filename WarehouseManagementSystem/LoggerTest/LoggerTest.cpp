#include "pch.h"
#include "CppUnitTest.h"
#include <fstream>
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
			char* filePath = "TestStuff.txt";
			Assert::AreEqual(0, logInit(filePath));
			logClose(filepath)
		}
		TEST_METHOD(InitFail)
		{
			char* filePath = "NoExist/";
			Assert::AreEqual(-1, logInit(filePath));
			logClose(filepath)
		}
		TEST_METHOD(WriteBufferOverflow)
		{
			char* overflowString = "adsjhfbsaodhifbSKDJFBLKSAjdfblksajxvblkJXbvlkasdbglkasjdhflkajdshflkajsdfblkasjdhflksajhdflksajdhflkasdjhflksadjhflksajhdflksajhdf";
			Assert::AreEqual(-1, writeToFile(overflowString));
			logClose(filepath)
		}
		TEST_METHOD(WriteNormal)
		{
			char* normalString = "Hello world";
			Assert::AreEqual(0, writeToFile(normalString));
			logClose(filepath)
		}
		TEST_METHOD(TransactionImport)
		{
			int id = 634200;
			int quantity = 69;
			int transactionInformation = 1; // 1 for import. -1 for export. 0 for order cancelled
			Assert::AreEqual(0, logTransaction(id, quantity, transactionInformation));
			logClose(filepath)
		}
		TEST_METHOD(TransactionExport)
		{
			int id = 634200;
			int quantity = 69;
			int transactionInformation = -1; // 1 for import. -1 for export. 0 for order cancelled
			Assert::AreEqual(0, logTransaction(id, quantity, transactionInformation));
			logClose(filepath)
		}
		TEST_METHOD(TransactionCancelled)
		{
			int id = 634200;
			int quantity = 69;
			int transactionInformation = 0; // 1 for import. -1 for export. 0 for order cancelled
			Assert::AreEqual(0, logTransaction(id, quantity, transactionInformation));
			logClose(filepath)
		}
		TEST_METHOD(TransactionFail)// i failed to get it to fail. ROFL
		{
			int id = 2140000000;
			int quantity = 2140000000;
			int transactionInformation = 89; // 1 for import. -1 for export. 0 for order cancelled
			Assert::AreEqual(-1, logTransaction(id, quantity, transactionInformation));
			logClose(filepath)
		}
	};
}
