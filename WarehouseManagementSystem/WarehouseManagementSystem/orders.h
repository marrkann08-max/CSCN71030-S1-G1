#ifndef ORDERS_H
#define ORDERS_H

#include "inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ORDER_RECEIVE 'R'
#define ORDER_DISPATCH 'D'
#define ORDER_SUCCESS 0
#define ORDER_FAILURE -1

typedef enum
{
    ORDER_NOT_STARTED = 0,
    ORDER_IN_PROCESS,
    ORDER_COMPLETED,
    ORDER_REJECTED
} OrderStatus;

typedef struct
{
    unsigned int productID;
    int quantity;
    char orderType;
    OrderStatus status;
} Order;

/* Matches Logger's supported logTransaction(id, amount, info) interface. */
typedef int (*OrderLogger)(int id, int amount, int info);

/**
 * Author: Inder
 * Input: A Logger-compatible transaction function, or NULL to disconnect it.
 * Output: No return value; subsequent Orders operations use the supplied logger.
 * Purpose: Integrate Orders with Logger while permitting isolated failure tests.
 */
void order_set_logger(OrderLogger logger);

/**
 * Author: Inder
 * Input: Product ID, positive quantity, and R (receive) or D (dispatch).
 * Output: A newly allocated valid Order, or NULL for invalid input/allocation failure.
 * Purpose: Validate and construct an order according to the Orders test plan.
 */
Order* order_create(unsigned int productID, int quantity, char orderType);

/**
 * Author: Inder
 * Input: Shared Inventory list head and a valid Order pointer.
 * Output: ORDER_SUCCESS or ORDER_FAILURE; updates Order status and stock atomically.
 * Purpose: Validate, log, and execute a receive or dispatch transaction.
 */
int order_process(Product* head, Order* order);

/**
 * Author: Inder
 * Input: Shared Inventory list, product ID, and positive receive quantity.
 * Output: ORDER_SUCCESS after increasing stock, otherwise ORDER_FAILURE unchanged.
 * Purpose: Apply a validated receive while preventing integer overflow.
 */
int processReceive(Product* head, unsigned int productID, int quantity);

/**
 * Author: Inder
 * Input: Shared Inventory list, product ID, and positive dispatch quantity.
 * Output: ORDER_SUCCESS after reducing stock, otherwise ORDER_FAILURE unchanged.
 * Purpose: Apply a validated dispatch while preventing insufficient stock.
 */
int processDispatch(Product* head, unsigned int productID, int quantity);

/**
 * Author: Inder
 * Input: An Order pointer, which may be NULL.
 * Output: No return value; allocated Order memory is released.
 * Purpose: Clean up an order safely.
 */
void order_free(Order* order);

/**
 * Author: Inder
 * Input: Shared Inventory list and deterministic simulation seed.
 * Output: ORDER_SUCCESS or ORDER_FAILURE from the normal Orders workflow.
 * Purpose: Simulate one reproducible order without bypassing validation or logging.
 */
int order_simulate(Product* head, unsigned int seed);

#ifdef __cplusplus
}
#endif

#endif
