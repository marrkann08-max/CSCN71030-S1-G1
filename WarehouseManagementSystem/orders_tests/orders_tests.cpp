#include "pch.h"
#include "CppUnitTest.h"
extern "C" {
#include"../WarehouseManagementSystem/orders.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace orderstests
{
	TEST_CLASS(orderstests)
	{
	public:

		TEST_METHOD(create_valid_receive_order)
		{
			Order* order = order_create(1001, 3, ORDER_RECEIVE);
			Assert::IsNotNull(order);
			Assert::AreEqual(1001U, order->productID);
			Assert::AreEqual(3, order->quantity);
			Assert::AreEqual((int)ORDER_RECEIVE, (int)order->orderType);
			Assert::AreEqual((int)ORDER_NOT_STARTED, (int)order->status);

			order_free(order);
		}
		TEST_METHOD(create_valid_dispatch_order) {
			Order* order = order_create(1001, 8, ORDER_DISPATCH);
			Assert::IsNotNull(order);
			Assert::AreEqual(1001U, order->productID);
			Assert::AreEqual(8, order->quantity);
			Assert::AreEqual((int)ORDER_DISPATCH, (int)order->orderType);
			Assert::AreEqual((int)ORDER_NOT_STARTED, (int)order->status);

			order_free(order);
		}
		TEST_METHOD(invalid_productID) {
			Order* order = order_create(0, 5, ORDER_RECEIVE);
			Assert::IsNotNull(order);
			order_free(order);
		}
		TEST_METHOD(test_zero_quantity) {
			Order* order = order_create(1001, 0, ORDER_RECEIVE);
			Assert::IsNotNull(order);
			order_free(order);
		}
		TEST_METHOD(negative_quantity) {
			Order* order = order_create(1001, -5, ORDER_RECEIVE);
			Assert::IsNotNull(order);
			order_free(order);
		}
		TEST_METHOD(invalid_orderType) {
			Order* order = order_create(1001, 5, 'X');
			Assert::IsNotNull(order);
			order_free(order);
		}
		TEST_METHOD(memory_release) {
			Order* order = order_create(1001, 4, ORDER_RECEIVE);
			Assert::IsNotNull(order);
			order_free(order);
		}
		TEST_METHOD(test_receive_stock) {
			Product product = { 1001, "Hammer", 10, "A-01", NULL };
			int result = processReceive(&product, 1001, 5);
			Assert::AreEqual(ORDER_SUCCESS, result);
			Assert::AreEqual(15, product.quantity);
		}
		TEST_METHOD(test_dispatch_stock) {
			Product product = { 1001, "Hammer", 10, "A-01", NULL };
			int result = processDispatch(&product, 1001, 5);
			Assert::AreEqual(ORDER_SUCCESS, result);
			Assert::AreEqual(5, product.quantity);

		}
		TEST_METHOD(test_receive_missing_stock) {
			Product product = { 1001, "Hammer", 10, "A-01", NULL };
			int result = processReceive(&product, 9999, 5);
			Assert::AreEqual(ORDER_FAILURE, result);
		}
		TEST_METHOD(test_dispatch_insufficient_stock) {
			Product product = { 1001, "Hammer", 4, "A-01", NULL };
			int result = processDispatch(&product, 1001, 10);
			Assert::AreEqual(ORDER_FAILURE, result);
			Assert::AreEqual(4, product.quantity);
		}
		TEST_METHOD(test_order_process_receive) {
			Product product = { 1001, "Hammer", 10, "A-01", NULL };
            Order* order = order_create(1001, 5, ORDER_RECEIVE);
			Assert::IsNotNull(order);
			int result = order_process(&product, order);
			Assert::AreEqual(ORDER_SUCCESS, result);
			Assert::AreEqual(15, product.quantity);
			Assert::AreEqual(static_cast<int>(ORDER_COMPLETED), static_cast<int>(order->status));
			order_free(order);
		}
		TEST_METHOD(test_order_process_dispatch) {
			Product product = { 1001, "Hammer", 10, "A-01", NULL };
			Order* order = order_create(1001, 3, ORDER_DISPATCH);
			Assert::IsNotNull(order);
			int result = order_process(&product, order);
			Assert::AreEqual(ORDER_SUCCESS, result);
			Assert::AreEqual(7, product.quantity);
			Assert::AreEqual(static_cast<int>(ORDER_COMPLETED), static_cast<int>(order->status));
			order_free(order);
		}
		TEST_METHOD(test_order_process_reject) {
			Product product = { 1001, "Hammer", 4 , "A-01", NULL };
			Order* order = order_create(1001, 10, ORDER_DISPATCH);
			Assert::IsNotNull(order);
			int result = order_process(&product, order);
			Assert::AreEqual(ORDER_FAILURE, result);
			Assert::AreEqual(4, product.quantity);
			Assert::AreEqual(static_cast<int>(ORDER_REJECTED), static_cast<int>(order->status));
			order_free(order);
		}
		TEST_METHOD(test_order_process_missing) {
			Product product = { 1001, "Hammer", 10, "A-01", NULL };
			Order* order = order_create(9999, 3, ORDER_RECEIVE);
			Assert::IsNotNull(order);
			int result = order_process(&product, order);
			Assert::AreEqual(ORDER_FAILURE, result);
			Assert::AreEqual(10, product.quantity);
			Assert::AreEqual(static_cast<int>(ORDER_REJECTED), static_cast<int>(order->status));
			order_free(order);
		}
		TEST_METHOD(test_logger_failure) {
			Product product = { 1001, "Hammer", 5, "A-01", NULL };
			Order* order = order_create(1001, 3, ORDER_RECEIVE);
			Assert::IsNotNull(order);
			int result = order_process(&product, order);
			Assert::AreEqual(ORDER_FAILURE, result);
			Assert::AreEqual(15, product.quantity);
			Assert::AreEqual(static_cast<int>(ORDER_REJECTED), static_cast<int>(order->status));
			order_free(order);
		}
		TEST_METHOD(test_receive_overflow) {
			Product product = { 1001, "Hammer", INT_MAX, "A-01", NULL };
			int result = processReceive(&product, 1001,1);
			Assert::AreEqual(ORDER_FAILURE, result);
			Assert::AreEqual(INT_MAX, product.quantity);
		}
		TEST_METHOD(test_atomic_transaction) {
			Product product = { 1001, "Hammer", 5, "A-01", NULL };
			Order* order = order_create(1001, 3, ORDER_RECEIVE);
			Assert::IsNotNull(order);
			int result = order_process(&product, order);
			Assert::AreEqual(ORDER_FAILURE, result);
			Assert::AreEqual(5, product.quantity);
			Assert::AreEqual(static_cast<int>(ORDER_REJECTED), static_cast<int>(order->status));
			order_free(order);
		}
		TEST_METHOD(order_simulate_test) {
			Product inventory1 = { 1001, "Hammer", 10, "A-01", NULL };
			Product inventory2 = { 1001, "Hammer", 10, "A-01", NULL };

			order_simulate(&inventory1, 2026);
			order_simulate(&inventory2, 2026);


			Assert::AreEqual(inventory1.quantity, inventory2.quantity);
		}
	};

}
