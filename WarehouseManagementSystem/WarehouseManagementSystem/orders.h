#pragma once
#ifndef ORDERS_H
#define ORDERS_H
#define ORDER_RECEIVE 'R'
#define ORDER_DISPATCH 'D'
#define ORDER_SUCCESS 0
#define ORDER_FAILURE -1
typedef enum {
	ORDER_NOT_STARTED = 0,ORDER_IN_PROCESS, ORDER_COMPLETED,ORDER_REJECTED
}orderstatus;
typedef struct {
	unsigned int productID;
	int quantity;
	char orderType;
	orderstatus status;
}Order;
Order* order_create(unsigned int productID, int quantity, char ordertype);
int order_process(Product* head, Order * order);
int processReceive(Product* head, unsigned int productID, int quantity);
int processDispatch(Product* head, unsigned int productID, int quantity);
void order_free(Order* order);
int order_simulate(Product* head, unsigned int seed);
#endif
