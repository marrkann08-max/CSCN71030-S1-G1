#include "inventory.h"
#include "utilities.h"

#include <stdlib.h>
#include <string.h>

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

/* Temporary TDD stub: implemented during INV-F-004. */
int inventory_delete_product(
    Product** head,
    unsigned int id
)
{
    (void)head;
    (void)id;

    return -1;
}

/* Temporary TDD stub: implemented during INV-F-005. */
void inventory_free_all(Product** head)
{
    (void)head;
}
