#pragma once
#ifndef ORDERS_H
#define ORDERS_H
#define ORDER_RECEIVE 'R'
#define ORDER_DISPATCH 'D'
#define ORDER_SUCCESS 0
#define ORDER_FAILURE -1
#define PRODUCT_NAME_LENGTH 64
#define PRODUCT_LOCATION_LENGTH 32
typedef enum {
	ORDER_NOT_STARTED = 0,ORDER_IN_PROCESS, ORDER_COMPLETED,ORDER_REJECTED
}orderstatus;
typedef struct {
	unsigned int productID;
	int quantity;
	char orderType;
	orderstatus status;
}Order;
typedef struct Product
{
    unsigned int id;
    char name[PRODUCT_NAME_LENGTH];
    int quantity;
    char location[PRODUCT_LOCATION_LENGTH];
    struct Product* next;
} Product;

Order* order_create(unsigned int productID, int quantity, char orderType);
int order_process(Product* head, Order * order);
int processReceive(Product* head, unsigned int productID, int quantity);
int processDispatch(Product* head, unsigned int productID, int quantity);
void order_free(Order* order);
int order_simulate(Product* head, unsigned int seed);
#endif
