#include "orders.h"
#include "utilities.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static OrderLogger transaction_logger = NULL;

/**
 * Author: Inder
 * Input: Product ID, quantity, and order type to validate.
 * Output: ORDER_SUCCESS only when every value satisfies Utilities and Orders rules.
 * Purpose: Centralize Orders validation without duplicating Utilities parsing logic.
 */
static int order_validate(unsigned int productID, int quantity, char orderType)
{
    char product_text[32];
    char quantity_text[32];
    char type_text[2] = { orderType, '\0' };
    unsigned int validated_product;
    int validated_quantity;

    if (snprintf(product_text, sizeof(product_text), "%u", productID) < 0 ||
        snprintf(quantity_text, sizeof(quantity_text), "%d", quantity) < 0 ||
        util_string_to_uint(product_text, 1U, UINT_MAX, &validated_product) != 0 ||
        util_string_to_int(quantity_text, 1, INT_MAX, &validated_quantity) != 0 ||
        util_validate_string(type_text, 1U) != 0 ||
        (orderType != ORDER_RECEIVE && orderType != ORDER_DISPATCH))
    {
        return ORDER_FAILURE;
    }

    return ORDER_SUCCESS;
}

/**
 * Author: Inder
 * Input: Product ID, quantity, and Logger transaction classification.
 * Output: Logger result, or ORDER_FAILURE when Logger is unavailable.
 * Purpose: Keep Orders dependent only on Logger's supported transaction signature.
 */
static int order_log(unsigned int productID, int quantity, int info)
{
    if (transaction_logger == NULL || productID > (unsigned int)INT_MAX)
    {
        return ORDER_FAILURE;
    }

    return transaction_logger((int)productID, quantity, info) == 0
        ? ORDER_SUCCESS
        : ORDER_FAILURE;
}

void order_set_logger(OrderLogger logger)
{
    transaction_logger = logger;
}

Order* order_create(unsigned int productID, int quantity, char orderType)
{
    Order* order;

    if (order_validate(productID, quantity, orderType) != ORDER_SUCCESS)
    {
        (void)order_log(productID, quantity, 0);
        return NULL;
    }

    order = (Order*)malloc(sizeof(Order));

    if (order == NULL)
    {
        (void)order_log(productID, quantity, 0);
        return NULL;
    }

    order->productID = productID;
    order->quantity = quantity;
    order->orderType = orderType;
    order->status = ORDER_NOT_STARTED;
    return order;
}

int order_process(Product* head, Order* order)
{
    Product* product;
    int new_quantity;
    int log_info;

    if (order == NULL)
    {
        return ORDER_FAILURE;
    }

    order->status = ORDER_IN_PROCESS;

    if (util_check_null(head) != 0 ||
        order_validate(order->productID, order->quantity, order->orderType) != ORDER_SUCCESS)
    {
        order->status = ORDER_REJECTED;
        (void)order_log(order->productID, order->quantity, 0);
        return ORDER_FAILURE;
    }

    product = inventory_get_product(head, order->productID);

    if (product == NULL || product->quantity < 0)
    {
        order->status = ORDER_REJECTED;
        (void)order_log(order->productID, order->quantity, 0);
        return ORDER_FAILURE;
    }

    if (order->orderType == ORDER_RECEIVE)
    {
        if (product->quantity > INT_MAX - order->quantity)
        {
            order->status = ORDER_REJECTED;
            (void)order_log(order->productID, order->quantity, 0);
            return ORDER_FAILURE;
        }

        new_quantity = product->quantity + order->quantity;
        log_info = 1;
    }
    else
    {
        if (product->quantity < order->quantity)
        {
            order->status = ORDER_REJECTED;
            (void)order_log(order->productID, order->quantity, 0);
            return ORDER_FAILURE;
        }

        new_quantity = product->quantity - order->quantity;
        log_info = -1;
    }

    /* Log first: inventory is not changed if Logger rejects the transaction. */
    if (order_log(order->productID, order->quantity, log_info) != ORDER_SUCCESS ||
        inventory_update_quantity(head, order->productID, new_quantity) != 0)
    {
        order->status = ORDER_REJECTED;
        return ORDER_FAILURE;
    }

    order->status = ORDER_COMPLETED;
    return ORDER_SUCCESS;
}

int processReceive(Product* head, unsigned int productID, int quantity)
{
    Product* product;

    if (util_check_null(head) != 0 ||
        order_validate(productID, quantity, ORDER_RECEIVE) != ORDER_SUCCESS)
    {
        return ORDER_FAILURE;
    }

    product = inventory_get_product(head, productID);

    if (product == NULL || product->quantity < 0 ||
        product->quantity > INT_MAX - quantity)
    {
        return ORDER_FAILURE;
    }

    return inventory_update_quantity(head, productID, product->quantity + quantity) == 0
        ? ORDER_SUCCESS
        : ORDER_FAILURE;
}

int processDispatch(Product* head, unsigned int productID, int quantity)
{
    Product* product;

    if (util_check_null(head) != 0 ||
        order_validate(productID, quantity, ORDER_DISPATCH) != ORDER_SUCCESS)
    {
        return ORDER_FAILURE;
    }

    product = inventory_get_product(head, productID);

    if (product == NULL || product->quantity < quantity)
    {
        return ORDER_FAILURE;
    }

    return inventory_update_quantity(head, productID, product->quantity - quantity) == 0
        ? ORDER_SUCCESS
        : ORDER_FAILURE;
}

void order_free(Order* order)
{
    free(order);
}

int order_simulate(Product* head, unsigned int seed)
{
    Product* selected;
    Order* order;
    unsigned int state;
    size_t product_count = 0U;
    size_t selected_index;
    int quantity;
    char type;
    int result;

    if (util_check_null(head) != 0)
    {
        return ORDER_FAILURE;
    }

    for (selected = head; selected != NULL; selected = selected->next)
    {
        ++product_count;
    }

    state = seed * 1664525U + 1013904223U;
    selected_index = (size_t)(state % (unsigned int)product_count);
    selected = head;

    while (selected_index-- > 0U)
    {
        selected = selected->next;
    }

    state = state * 1664525U + 1013904223U;
    quantity = (int)(state % 10U) + 1;
    state = state * 1664525U + 1013904223U;
    type = (state % 2U == 0U) ? ORDER_RECEIVE : ORDER_DISPATCH;

    order = order_create(selected->id, quantity, type);

    if (order == NULL)
    {
        return ORDER_FAILURE;
    }

    result = order_process(head, order);
    order_free(order);
    return result;
}
