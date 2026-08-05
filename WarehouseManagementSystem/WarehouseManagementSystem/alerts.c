#include "alerts.h"

#include <stdio.h>

#define PRODUCT_NAME_CAPACITY 64U
#define PRODUCT_LOCATION_CAPACITY 32U

/* Matches the shared Inventory module's public Product layout. */
struct Product
{
    unsigned int id;
    char name[PRODUCT_NAME_CAPACITY];
    int quantity;
    char location[PRODUCT_LOCATION_CAPACITY];
    struct Product* next;
};

int check_low_stock(
    const Product* head,
    int threshold,
    const char* file_path
)
{
    const Product* current;
    FILE* file = NULL;
    int flagged_count = 0;

    if (file_path == NULL)
    {
        return -1;
    }

    if (fopen_s(&file, file_path, "w") != 0 || file == NULL)
    {
        return -1;
    }

    if (fprintf(
            file,
            "Low-Stock Alert Report\n"
            "Threshold: %d\n"
            "========================================\n",
            threshold
        ) < 0)
    {
        fclose(file);
        return -1;
    }

    current = head;
    while (current != NULL)
    {
        if (current->quantity < threshold)
        {
            if (fprintf(
                    file,
                    "Product ID: %u | Name: %s | Quantity: %d\n",
                    current->id,
                    current->name,
                    current->quantity
                ) < 0)
            {
                fclose(file);
                return -1;
            }

            ++flagged_count;
        }

        current = current->next;
    }

    if (fclose(file) != 0)
    {
        return -1;
    }

    return flagged_count;
}
