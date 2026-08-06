#ifndef REPORT_H
#define REPORT_H

#include "inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a timestamped, human-readable inventory report in directory.
 * An empty inventory produces a report containing only the headings.
 *
 * @return 0 on success, or -1 for invalid input or a file-system error.
 */
int report_generate_text(Product** head, const char* directory);

/**
 * Creates a timestamped CSV inventory report in directory.
 * Fields containing commas, quotes, or line breaks are escaped correctly.
 *
 * @return 0 on success, or -1 for invalid input or a file-system error.
 */
int report_generate_csv(Product** head, const char* directory);

#ifdef __cplusplus
}
#endif

#endif
