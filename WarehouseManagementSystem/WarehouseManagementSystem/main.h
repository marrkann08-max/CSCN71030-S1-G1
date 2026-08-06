//--------------------------------------------------------------------------
// main.h - Main module interface for the Warehouse Management System
//          Author: Ivan Immanuel Shaji
//--------------------------------------------------------------------------

#ifndef MAIN_H
#define MAIN_H

#include "config.h"
#include "inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MENU_INPUT_CAPACITY 128U
#define MAIN_LOG_FILE_PATH "warehouse.log"
#define MAIN_ALERT_FILE_PATH "low_stock.txt"
#define MAIN_TEXT_REPORT_PATH "inventory_report.txt"
#define MAIN_CSV_REPORT_PATH "inventory_report.csv"

typedef enum MenuChoice
{
    MENU_EXIT = 0,
    MENU_INVENTORY,
    MENU_ORDERS,
    MENU_SEARCH,
    MENU_REPORTS,
    MENU_ALERTS
} MenuChoice;

/*
 * Author: Ivan Immanuel Shaji
 * Input: None.
 * Output: Writes the available menu choices to standard output.
 * Purpose: Display the Warehouse Management System menu.
 */
void display_menu(void);

/*
 * Author: Ivan Immanuel Shaji
 * Input: Editable menu text and a MenuChoice output pointer.
 * Output: Stores a validated menu choice and returns 0, otherwise -1.
 * Purpose: Convert validated character input into a menu selection.
 */
int parse_menu_choice(char* input, MenuChoice* out_choice);

/*
 * Author: Ivan Immanuel Shaji
 * Input: A MenuChoice output pointer.
 * Output: Reads and stores one validated selection, returning 0 or -1.
 * Purpose: Read one menu selection safely from standard input.
 */
int read_menu_choice(MenuChoice* out_choice);

/*
 * Author: Ivan Immanuel Shaji
 * Input: Selected menu value, shared Inventory pointer, and Config pointer.
 * Output: Calls the selected module and returns its success or failure status.
 * Purpose: Centralize switch-based dispatch and module status checking.
 */
int dispatch_menu_option(
    int choice,
    Product** head,
    const Config* config
);

/*
 * Author: Ivan Immanuel Shaji
 * Input: Shared Inventory pointer and read-only Config pointer.
 * Output: Runs until Exit/EOF and returns 0, or -1 for an input failure.
 * Purpose: Control the validated interactive application menu loop.
 */
int run_menu(Product** head, const Config* config);

#ifdef __cplusplus
}
#endif

#endif
