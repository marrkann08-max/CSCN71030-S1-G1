#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include"../WarehouseManagementSystem/orders.h"
Order* order_create(unsigned int productID, int quantity, char orderType) {
    Order* order = (Order*)malloc(sizeof(Order));
    if (order == NULL) {
        return NULL; /* only a real allocation failure returns NULL */
    }

    order->productID = productID;
    order->quantity = quantity;
    order->orderType = orderType;
    order->status = ORDER_NOT_STARTED;
    return order;
}
int order_process(Product* head, Order* order) {
    int result;
    if (head == NULL || order == NULL) {
        return ORDER_FAILURE;
    }
    order->status = ORDER_IN_PROCESS;
    if (order->quantity <= 0 ||
        (order->orderType != ORDER_RECEIVE && order->orderType != ORDER_DISPATCH)) {
        order->status = ORDER_REJECTED;
        return ORDER_FAILURE;
    }
    if (order->orderType == ORDER_RECEIVE) {
        result = processReceive(head, order->productID, order->quantity);
    }
    else if (order->orderType == ORDER_DISPATCH) {
        result = processDispatch(head, order->productID, order->quantity);
    }
    else {
        order->status = ORDER_REJECTED;
        return ORDER_FAILURE;
    }
    if (result == ORDER_SUCCESS) {
        order->status = ORDER_COMPLETED;
        return ORDER_SUCCESS;
    }
    order->status = ORDER_REJECTED;
    return ORDER_FAILURE;

}
int processReceive(Product* head, unsigned int productID, int quantity) {
    Product* current = head;
    if (head == NULL || quantity <= 0) {
        return ORDER_FAILURE;
    }
    while (current != NULL) {
        if (current->id == productID) {
            if (current->quantity > INT_MAX - quantity) {
                return ORDER_FAILURE;
            }
            current->quantity += quantity;
            return ORDER_SUCCESS;
        }
        current = current->next;
    }
    return ORDER_FAILURE;
}
int processDispatch(Product* head, unsigned int productID, int quantity) {
    Product* current = head;
    if (head == NULL || quantity <= 0) {
        return ORDER_FAILURE;
    }
    while (current != NULL) {
        if (current->id == productID) {
            if (current->quantity < quantity) {
                return ORDER_FAILURE;
            }
            current->quantity -= quantity;
            return ORDER_SUCCESS;
        }
        current = current->next;
    }
    return ORDER_FAILURE;
}
void order_free(Order* order) {
    if (order != NULL)
    {
        free(order);
    }

}
int order_simulate(Product* head, unsigned int seed) {
    if (head == NULL) {
        return -1;
    }
    srand(seed);
    int quantity = (rand() % 10) + 1;
    char type = (rand() % 2 == 0) ? ORDER_RECEIVE : ORDER_DISPATCH;
    return (type == ORDER_RECEIVE)? processReceive(head, head->id, quantity): processDispatch(head, head->id, quantity);
}
int main(void) {

}