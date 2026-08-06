#include "inventory.h"
#include "utilities.h"

#include <stdlib.h>
#include <string.h>

/*
 * Author: Keshav Kumar Markan
 * Input: Shared Inventory head and Product ID.
 * Output: Returns the matching Product pointer, or NULL if it is not found.
 * Purpose: Retrieve a Product from the Inventory by its unique ID.
 */
Product* inventory_get_product(
    Product* head,
    unsigned int id
)
{
    Product* current = head;

    while (current != NULL)
    {
        if (current->id == id)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

/*
 * Author: Keshav Kumar Markan
 * Input: Inventory head, Product ID, name, quantity, and location.
 * Output: Adds a Product and returns 0, or returns -1 if it cannot be added.
 * Purpose: Validate, allocate, and store a Product in the linked list.
 */
int inventory_add_product(
    Product** head,
    unsigned int id,
    const char* name,
    int quantity,
    const char* location
)
{
    Product* new_product;
    Product* current;
    char validated_name[PRODUCT_NAME_LENGTH];
    char validated_location[PRODUCT_LOCATION_LENGTH];
    size_t name_length;
    size_t location_length;

    if (util_check_null(head) != 0 ||
        util_check_null(name) != 0 ||
        util_check_null(location) != 0 ||
        id == 0U ||
        quantity < 0)
    {
        return -1;
    }

    if (inventory_get_product(*head, id) != NULL)
    {
        return -1;
    }

    name_length = strlen(name);

    if (name_length >= PRODUCT_NAME_LENGTH)
    {
        name_length = PRODUCT_NAME_LENGTH - 1U;
    }

    memcpy(validated_name, name, name_length);
    validated_name[name_length] = '\0';

    location_length = strlen(location);

    if (location_length >= PRODUCT_LOCATION_LENGTH)
    {
        location_length = PRODUCT_LOCATION_LENGTH - 1U;
    }

    memcpy(validated_location, location, location_length);
    validated_location[location_length] = '\0';

    if (util_validate_string(
        validated_name,
        PRODUCT_NAME_LENGTH - 1U
    ) != 0 ||
        util_validate_string(
            validated_location,
            PRODUCT_LOCATION_LENGTH - 1U
        ) != 0)
    {
        return -1;
    }

    new_product = malloc(sizeof(Product));

    if (new_product == NULL)
    {
        return -1;
    }

    new_product->id = id;
    memcpy(
        new_product->name,
        validated_name,
        strlen(validated_name) + 1U
    );
    new_product->quantity = quantity;
    memcpy(
        new_product->location,
        validated_location,
        strlen(validated_location) + 1U
    );
    new_product->next = NULL;

    if (*head == NULL)
    {
        *head = new_product;
        return 0;
    }

    current = *head;

    while (current->next != NULL)
    {
        current = current->next;
    }

    current->next = new_product;

    return 0;
}

/*
 * Author: Keshav Kumar Markan
 * Input: Inventory head, Product ID, and new quantity.
 * Output: Updates the matching quantity and returns 0, otherwise -1.
 * Purpose: Change the stock quantity of an existing Product.
 */
int inventory_update_quantity(
    Product* head,
    unsigned int id,
    int new_quantity
)
{
    Product* product;

    if (util_check_null(head) != 0 ||
        id == 0U ||
        new_quantity < 0)
    {
        return -1;
    }

    product = inventory_get_product(head, id);

    if (product == NULL)
    {
        return -1;
    }

    product->quantity = new_quantity;

    return 0;
}

/*
 * Author: Keshav Kumar Markan
 * Input: Address of the Inventory head and Product ID.
 * Output: Removes the matching Product and returns 0, otherwise -1.
 * Purpose: Delete one Product and release its allocated memory.
 */
int inventory_delete_product(
    Product** head,
    unsigned int id
)
{
    Product* current;
    Product* previous;

    if (util_check_null(head) != 0 ||
        id == 0U ||
        *head == NULL)
    {
        return -1;
    }

    current = *head;
    previous = NULL;

    while (current != NULL && current->id != id)
    {
        previous = current;
        current = current->next;
    }

    if (current == NULL)
    {
        return -1;
    }

    if (previous == NULL)
    {
        *head = current->next;
    }
    else
    {
        previous->next = current->next;
    }

    free(current);

    return 0;
}

/*
 * Author: Keshav Kumar Markan
 * Input: Address of the Inventory head pointer.
 * Output: Frees every Product and sets the head pointer to NULL.
 * Purpose: Release all Inventory memory during cleanup.
 */
void inventory_free_all(Product** head)
{
    Product* current;
    Product* next_product;

    if (head == NULL)
    {
        return;
    }

    current = *head;

    while (current != NULL)
    {
        next_product = current->next;
        free(current);
        current = next_product;
    }

    *head = NULL;
}
