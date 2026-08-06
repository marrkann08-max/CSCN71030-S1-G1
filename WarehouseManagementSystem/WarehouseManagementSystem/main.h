//--------------------------------------------------------------------------
//		main.h - Header file for the Warehouse Management System
//						Author: Ivan Immanuel Shaji
//--------------------------------------------------------------------------

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define MENU_INPUT_CAPACITY 64U

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

#ifdef __cplusplus
}
#endif