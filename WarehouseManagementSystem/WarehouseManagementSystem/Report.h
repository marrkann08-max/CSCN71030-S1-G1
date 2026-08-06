#ifndef REPORT_H
#define REPORT_H

#include "inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Author: Alex English and Ivan Immanuel Shaji
 * Input: Read-only inventory list and caller-supplied text-file path.
 * Output: Writes a timestamped inventory report and returns 0, otherwise -1.
 * Purpose: Generate the complete human-readable inventory report.
 */
int report_generate_text(const Product* head, const char* file_path);

/*
 * Author: Alex English and Ivan Immanuel Shaji
 * Input: Read-only inventory list and caller-supplied CSV-file path.
 * Output: Writes a CSV inventory report and returns 0, otherwise -1.
 * Purpose: Export the complete inventory for use by external tools.
 */
int report_generate_csv(const Product* head, const char* file_path);

#ifdef __cplusplus
}
#endif

#endif
