#include "Logger.h"
#include "utilities.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define LOGGER_PATH_CAPACITY 260U
#define LOGGER_TIMESTAMP_CAPACITY 32U
#define LOGGER_RECORD_CAPACITY 768U

static FILE* log_file = NULL;
static char logger_username[LOGGER_USERNAME_CAPACITY] = { 0 };

/*
 * Author: Alex and Keshav Kumar Markan
 * Input: Read-only text, destination buffer, and destination capacity.
 * Output: Stores a validated local copy without changing caller data.
 * Purpose: Apply shared Utilities validation to Logger string inputs.
 */
static int copy_validated_string(
    const char* source,
    char* destination,
    size_t capacity
)
{
    size_t length;

    if (util_check_null(source) != 0 ||
        util_check_null(destination) != 0 ||
        capacity < 2U)
    {
        return -1;
    }

    length = strlen(source);

    if (length == 0U || length >= capacity)
    {
        return -1;
    }

    memcpy(destination, source, length + 1U);

    return util_validate_string(destination, capacity - 1U);
}

/*
 * Author: Alex and Keshav Kumar Markan
 * Input: A validated message and initialized Logger state.
 * Output: Appends and flushes one timestamped log record.
 * Purpose: Centralize bounded formatting and file-error propagation.
 */
static int write_record(const char* message)
{
    time_t current_time;
    struct tm local_time;
    char timestamp[LOGGER_TIMESTAMP_CAPACITY];
    char record[LOGGER_RECORD_CAPACITY];
    int written;

    if (log_file == NULL || util_check_null(message) != 0)
    {
        return -1;
    }

    current_time = time(NULL);

    if (current_time == (time_t)-1 || localtime_s(&local_time, &current_time) != 0)
    {
        return -1;
    }

    if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &local_time) == 0U)
    {
        return -1;
    }

    written = snprintf(
        record,
        sizeof(record),
        "<%s> %s: %s\n",
        timestamp,
        logger_username,
        message
    );

    if (written < 0 || (size_t)written >= sizeof(record))
    {
        return -1;
    }

    if (fputs(record, log_file) == EOF || fflush(log_file) != 0)
    {
        return -1;
    }

    return 0;
}

/*
 * Author: Alex English and Keshav Kumar Markan
 * Input: Writable log-file path and authenticated username.
 * Output: Opens the log and returns 0, or returns -1 on failure.
 * Purpose: Start the Logger session before other logging operations.
 */
int logger_initialize(const char* file_path, const char* username)
{
    char validated_path[LOGGER_PATH_CAPACITY];
    char validated_username[LOGGER_USERNAME_CAPACITY];
    FILE* opened_file = NULL;

    if (log_file != NULL ||
        copy_validated_string(file_path, validated_path, sizeof(validated_path)) != 0 ||
        copy_validated_string(username, validated_username, sizeof(validated_username)) != 0)
    {
        return -1;
    }

    if (fopen_s(&opened_file, validated_path, "a") != 0 || opened_file == NULL)
    {
        return -1;
    }

    log_file = opened_file;
    memcpy(logger_username, validated_username, strlen(validated_username) + 1U);

    if (write_record("Program starting...") != 0)
    {
        fclose(log_file);
        log_file = NULL;
        memset(logger_username, 0, sizeof(logger_username));
        return -1;
    }

    return 0;
}

/*
 * Author: Alex English and Keshav Kumar Markan
 * Input: Validated message text.
 * Output: Appends the message and returns 0, otherwise -1.
 * Purpose: Record general application activity in the session log.
 */
int logger_write_string(const char* text)
{
    char validated_text[LOGGER_MESSAGE_CAPACITY];

    if (log_file == NULL ||
        copy_validated_string(text, validated_text, sizeof(validated_text)) != 0)
    {
        return -1;
    }

    return write_record(validated_text);
}

/*
 * Author: Alex English and Keshav Kumar Markan
 * Input: Product details, transaction type, and approval status.
 * Output: Appends a transaction entry and returns 0, otherwise -1.
 * Purpose: Record approved and rejected warehouse transactions.
 */
int logger_write_transaction(
    unsigned int product_id,
    const char* product_name,
    int quantity,
    char transaction_type,
    int approved
)
{
    char validated_name[LOGGER_PRODUCT_NAME_CAPACITY];
    char message[LOGGER_MESSAGE_CAPACITY];
    const char* type_text;
    const char* status_text;
    int written;

    if (log_file == NULL ||
        product_id == 0U ||
        quantity <= 0 ||
        (transaction_type != LOGGER_TRANSACTION_RECEIVE &&
         transaction_type != LOGGER_TRANSACTION_DISPATCH) ||
        (approved != 0 && approved != 1) ||
        copy_validated_string(product_name, validated_name, sizeof(validated_name)) != 0)
    {
        return -1;
    }

    type_text = transaction_type == LOGGER_TRANSACTION_RECEIVE
        ? "Receive"
        : "Dispatch";
    status_text = approved == 1 ? "Approved" : "Rejected";

    written = snprintf(
        message,
        sizeof(message),
        "Transaction | Product ID: %u | Product: %s | Quantity: %d | Type: %s | Status: %s",
        product_id,
        validated_name,
        quantity,
        type_text,
        status_text
    );

    if (written < 0 || (size_t)written >= sizeof(message))
    {
        return -1;
    }

    return write_record(message);
}

/*
 * Author: Alex English and Keshav Kumar Markan
 * Input: Current Logger session state.
 * Output: Writes the closing entry, closes the file, and clears the state.
 * Purpose: End the Logger session safely during program cleanup.
 */
void logger_shutdown(void)
{
    FILE* file_to_close;

    if (log_file == NULL)
    {
        return;
    }

    (void)write_record("Program closing...");

    file_to_close = log_file;
    log_file = NULL;
    memset(logger_username, 0, sizeof(logger_username));
    (void)fclose(file_to_close);
}
