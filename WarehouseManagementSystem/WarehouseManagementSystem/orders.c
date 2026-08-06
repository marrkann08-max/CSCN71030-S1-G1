#include "orders.h"
#include "Logger.h"
#include "utilities.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static OrderLogger transaction_logger = logger_write_transaction;

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
 * Input: Product details, transaction type, and approval status.
 * Output: Logger result, or ORDER_FAILURE when Logger is unavailable.
 * Purpose: Keep Orders dependent only on Logger's supported transaction signature.
 */
static int order_log(
    unsigned int productID,
    const char* productName,
    int quantity,
    char orderType,
    int approved
)
{
    if (transaction_logger == NULL)
    {
        return ORDER_FAILURE;
    }

    return transaction_logger(
        productID,
        productName,
        quantity,
        orderType,
        approved
    ) == 0
        ? ORDER_SUCCESS
        : ORDER_FAILURE;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Logger-compatible transaction function, or NULL.
 * Output: Stores the Logger function for later Orders operations.
 * Purpose: Connect or disconnect Orders transaction logging.
 */
void order_set_logger(OrderLogger logger)
{
    transaction_logger = logger;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Product ID, positive quantity, and receive or dispatch type.
 * Output: Returns a new Order pointer, or NULL for invalid input/failure.
 * Purpose: Validate and dynamically allocate an Order.
 */
Order* order_create(unsigned int productID, int quantity, char orderType)
{
    Order* order;

    if (order_validate(productID, quantity, orderType) != ORDER_SUCCESS)
    {
        return NULL;
    }

    order = (Order*)malloc(sizeof(Order));

    if (order == NULL)
    {
        return NULL;
    }

    order->productID = productID;
    order->quantity = quantity;
    order->orderType = orderType;
    order->status = ORDER_NOT_STARTED;
    return order;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory head and valid Order pointer.
 * Output: Updates Order status and returns success or failure.
 * Purpose: Validate, log, and apply an Order to Inventory safely.
 */
int order_process(Product* head, Order* order)
{
    Product* product;
    int new_quantity;

    if (order == NULL)
    {
        return ORDER_FAILURE;
    }

    order->status = ORDER_IN_PROCESS;

    if (util_check_null(head) != 0 ||
        order_validate(order->productID, order->quantity, order->orderType) != ORDER_SUCCESS)
    {
        order->status = ORDER_REJECTED;
        return ORDER_FAILURE;
    }

    product = inventory_get_product(head, order->productID);

    if (product == NULL || product->quantity < 0)
    {
        order->status = ORDER_REJECTED;
        (void)order_log(
            order->productID,
            "Unknown Product",
            order->quantity,
            order->orderType,
            0
        );
        return ORDER_FAILURE;
    }

    if (order->orderType == ORDER_RECEIVE)
    {
        if (product->quantity > INT_MAX - order->quantity)
        {
            order->status = ORDER_REJECTED;
            (void)order_log(
                order->productID,
                product->name,
                order->quantity,
                order->orderType,
                0
            );
            return ORDER_FAILURE;
        }

        new_quantity = product->quantity + order->quantity;
    }
    else
    {
        if (product->quantity < order->quantity)
        {
            order->status = ORDER_REJECTED;
            (void)order_log(
                order->productID,
                product->name,
                order->quantity,
                order->orderType,
                0
            );
            return ORDER_FAILURE;
        }

        new_quantity = product->quantity - order->quantity;
    }

    /* Log first: inventory is not changed if Logger rejects the transaction. */
    if (order_log(
            order->productID,
            product->name,
            order->quantity,
            order->orderType,
            1
        ) != ORDER_SUCCESS ||
        inventory_update_quantity(head, order->productID, new_quantity) != 0)
    {
        order->status = ORDER_REJECTED;
        return ORDER_FAILURE;
    }

    order->status = ORDER_COMPLETED;
    return ORDER_SUCCESS;
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory, Product ID, and positive receive quantity.
 * Output: Increases stock and returns success, otherwise failure.
 * Purpose: Apply a validated receive operation without overflow.
 */
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

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory, Product ID, and positive dispatch quantity.
 * Output: Reduces stock and returns success, otherwise failure.
 * Purpose: Apply a validated dispatch when enough stock is available.
 */
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

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Order pointer, which may be NULL.
 * Output: Releases the Order memory.
 * Purpose: Clean up a dynamically allocated Order safely.
 */
void order_free(Order* order)
{
    free(order);
}

/*
 * Author: Inderpreet Kaur Hundal
 * Input: Shared Inventory and deterministic Config seed.
 * Output: Processes one simulated Order and returns success or failure.
 * Purpose: Run a reproducible Order through the normal Orders workflow.
 */
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
