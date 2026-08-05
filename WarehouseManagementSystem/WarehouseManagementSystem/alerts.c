#include "alerts.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#define PRODUCT_NAME_CAPACITY 64U
#define PRODUCT_LOCATION_CAPACITY 32U
#define ALERT_PATH_CAPACITY 512U

/* Matches the shared Inventory module's public Product layout. */
struct Product
{
    unsigned int id;
    char name[PRODUCT_NAME_CAPACITY];
    int quantity;
    char location[PRODUCT_LOCATION_CAPACITY];
    struct Product* next;
};

static int copy_alert_path(
    char destination[ALERT_PATH_CAPACITY],
    const char* file_path
)
{
    size_t begin = 0U;
    size_t end;
    size_t length = 0U;

    if (file_path == NULL)
    {
        return -1;
    }

    while (length < ALERT_PATH_CAPACITY && file_path[length] != '\0')
    {
        ++length;
    }

    if (length == 0U || length == ALERT_PATH_CAPACITY)
    {
        return -1;
    }

    while (begin < length && isspace((unsigned char)file_path[begin]))
    {
        ++begin;
    }

    end = length;
    while (end > begin && isspace((unsigned char)file_path[end - 1U]))
    {
        --end;
    }

    if (begin == end)
    {
        return -1;
    }

    memcpy(destination, file_path + begin, end - begin);
    destination[end - begin] = '\0';
    return 0;
}

static int write_low_stock_record(
    FILE* stream,
    const Product* product
)
{
    if (stream == NULL || product == NULL)
    {
        return -1;
    }

    return fprintf(
        stream,
        "Product ID: %u | Name: %s | Quantity: %d\n",
        product->id,
        product->name,
        product->quantity
    ) < 0 ? -1 : 0;
}

int check_low_stock(
    const Product* head,
    int threshold,
    const char* file_path
)
{
    char validated_path[ALERT_PATH_CAPACITY];
    const Product* current;
    FILE* file = NULL;
    int flagged_count = 0;

    if (threshold <= 0 ||
        copy_alert_path(validated_path, file_path) != 0)
    {
        return -1;
    }

    if (fopen_s(&file, validated_path, "w") != 0 || file == NULL)
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
            if (flagged_count == INT_MAX ||
                write_low_stock_record(file, current) != 0 ||
                write_low_stock_record(stdout, current) != 0)
            {
                fclose(file);
                return -1;
            }

            ++flagged_count;
        }

        current = current->next;
    }

    if (flagged_count == 0 &&
        fputs(
            "No products are below the configured threshold.\n",
            file
        ) == EOF)
    {
        fclose(file);
        return -1;
    }

    if (flagged_count == 0 &&
        fputs(
            "No products are below the configured threshold.\n",
            stdout
        ) == EOF)
    {
        fclose(file);
        return -1;
    }

    if (fclose(file) != 0)
    {
        return -1;
    }

    return flagged_count;
}
