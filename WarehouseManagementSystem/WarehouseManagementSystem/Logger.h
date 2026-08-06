#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#define LOGGER_USERNAME_CAPACITY 64U
#define LOGGER_PRODUCT_NAME_CAPACITY 64U
#define LOGGER_MESSAGE_CAPACITY 512U
#define LOGGER_TRANSACTION_RECEIVE 'R'
#define LOGGER_TRANSACTION_DISPATCH 'D'

/*
 * Author: Alex and Keshav Kumar Markan
 * Input: A writable log-file path and the authenticated username.
 * Output: Opens the append-only session log and writes a startup entry.
 * Purpose: Initialize Logger before any other logging operation.
 */
int logger_initialize(const char* file_path, const char* username);

/*
 * Author: Alex and Keshav Kumar Markan
 * Input: A validated text message.
 * Output: Appends a timestamped entry for the authenticated user.
 * Purpose: Record general application activity safely.
 */
int logger_write_string(const char* text);

/*
 * Author: Alex and Keshav Kumar Markan
 * Input: Product details, transaction type, and approval status.
 * Output: Appends one complete timestamped transaction entry.
 * Purpose: Record approved and rejected warehouse transactions.
 */
int logger_write_transaction(
    unsigned int product_id,
    const char* product_name,
    int quantity,
    char transaction_type,
    int approved
);

/*
 * Author: Alex and Keshav Kumar Markan
 * Input: The current module-local Logger state.
 * Output: Writes the shutdown entry, closes the file, and clears state.
 * Purpose: End a Logger session safely; repeated calls are harmless.
 */
void logger_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
