#include "alerts.h"
#include "utilities.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#define ALERT_PATH_CAPACITY 512U

/*
 * Author: Navkirat Kaur
 * Input: Writable destination and a source report-file path.
 * Output: Stores a validated path and returns 0, or returns -1.
 * Purpose: Validate report paths using the shared Utilities module.
 */
static int copy_alert_path(
    char destination[ALERT_PATH_CAPACITY],
    const char* file_path
)
{
    size_t length = 0U;

    if (util_check_null(destination) != 0 ||
        util_check_null(file_path) != 0)
    {
        return -1;
    }

    while (length < ALERT_PATH_CAPACITY && file_path[length] != '\0')
    {
        ++length;
    }

    if (length == ALERT_PATH_CAPACITY)
    {
        return -1;
    }

    memcpy(destination, file_path, length + 1U);
    return util_validate_string(destination, ALERT_PATH_CAPACITY - 1U);
}

/*
 * Author: Navkirat Kaur
 * Input: Open output stream and one Inventory Product.
 * Output: Writes one record and returns 0, or returns -1.
 * Purpose: Produce a consistent low-stock record without changing Product.
 */
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

/*
 * Author: Navkirat Kaur
 * Input: Inventory head, positive threshold, and writable report path.
 * Output: Returns the number of low-stock products, or -1 on failure.
 * Purpose: Write low-stock warnings while leaving Inventory unchanged.
 */
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

    if (util_check_null(head) != 0)
    {
        fprintf(stderr, "Invalid Inventory pointer.\n");
        return -1;
    }

    if (threshold <= 0)
    {
        fprintf(stderr, "Invalid stock threshold.\n");
        return -1;
    }

    if (copy_alert_path(validated_path, file_path) != 0)
    {
        fprintf(stderr, "Invalid alert file path.\n");
        return -1;
    }

    if (fopen_s(&file, validated_path, "w") != 0 || file == NULL)
    {
        fprintf(stderr, "Unable to open alert report file.\n");
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
