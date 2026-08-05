#ifndef ALERTS_H
#define ALERTS_H

#include "inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Author: Keshav Kumar Markan
 * Input: Inventory head, positive threshold, and writable report path.
 * Output: Returns the number of low-stock products, or -1 on failure.
 * Purpose: Report products whose quantities are below the threshold.
 */
int check_low_stock(
    const Product* head,
    int threshold,
    const char* file_path
);

#ifdef __cplusplus
}
#endif

#endif
