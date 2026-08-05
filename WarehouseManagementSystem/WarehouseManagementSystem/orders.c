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
    return -1;
}
int processReceive(Product* head, unsigned int productID, int quantity) {
    Product* current = head;
    if (head == NULL || quantity <= 0) {
        return ORDER_FAILURE;
    }
    while (current != NULL) {
        if (current->id == productID) {
            current->quantity += quantity;
            return ORDER_SUCCESS;
        }
        current = current->next;
    }
    return ORDER_FAILURE;
}
int processDispatch(Product* head, unsigned int productID, int quantity) {
    return 0;
}
void order_free(Order* order) {
    if (order != NULL)
    {
        free(order);
    }

}
int order_simulate(Product* head, unsigned int seed) {
    return -1;
}
int main(void) {
    return 0;
}