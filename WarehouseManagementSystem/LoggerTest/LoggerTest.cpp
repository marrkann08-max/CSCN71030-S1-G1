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
			logCleanUp();
		}
		TEST_METHOD(InitFail)
		{
			char* filePath = "NoExist/";
			Assert::AreEqual(-1, logInit(filePath));
		}
		TEST_METHOD(WriteBufferOverflow)
		{
			char* filePath = "TestStuff.txt";
			logInit(filePath);
			char* overflowString = "adsjhfbsaodhifbSKDJFBLKSAjdfblksajxvblkJXbvlkasdbglkasjdhflkajdshflkajsdfblkasjdhflksajhdflksajdhflkasdjhflksadjhflksajhdflksajhdf";
			Assert::AreEqual(-1, writeToFile(overflowString));
		}
		TEST_METHOD(WriteNormal)
		{
			char* filePath = "TestStuff.txt";
			logInit(filePath);
			char* normalString = "Hello world";
			Assert::AreEqual(0, writeToFile(normalString));
			logCleanUp();
		}
		TEST_METHOD(TransactionImport)
		{
			char* filePath = "TestStuff.txt";
			logInit(filePath);
			int id = 634200;
			int quantity = 69;
			int transactionInformation = 1; // 1 for import. -1 for export. 0 for order cancelled
			Assert::AreEqual(0, logTransaction(id, quantity, transactionInformation));
			logCleanUp();
		}
		TEST_METHOD(TransactionExport)
		{
			char* filePath = "TestStuff.txt";
			logInit(filePath);
			int id = 634200;
			int quantity = 69;
			int transactionInformation = -1; // 1 for import. -1 for export. 0 for order cancelled
			Assert::AreEqual(0, logTransaction(id, quantity, transactionInformation));
			logCleanUp();
		}
		TEST_METHOD(TransactionCancelled)
		{
			char* filePath = "TestStuff.txt";
			logInit(filePath);
			int id = 634200;
			int quantity = 69;
			int transactionInformation = 0; // 1 for import. -1 for export. 0 for order cancelled
			Assert::AreEqual(0, logTransaction(id, quantity, transactionInformation));
			logCleanUp();
		}
	};
}
