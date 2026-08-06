//--------------------------------------------------------------------------
// main.c - Entry point for the Warehouse Management System
//                    Author: Ivan Immanuel Shaji
//--------------------------------------------------------------------------

#include "main.h"
#include "config.h"
#include "utilities.h"
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

/*
 * Author: Ivan Immanuel Shaji
 * Input: Characters remaining in standard input.
 * Output: Removes characters through the next newline or end of file.
 * Purpose: Recover safely after an overlength menu entry.
 */
static void discard_input_remainder(void)
{
	int character;

	do
	{
		character = getchar();
	} while (character != '\n' && character != EOF);
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Startup argument count and startup argument array.
 * Output: Returns EXIT_SUCCESS after a clean exit, otherwise EXIT_FAILURE.
 * Purpose: Load configuration and control the Main menu loop.
 */
int main(int argc, char* argv[])
{
	Config config;
	MenuChoice choice;
	int running = 1;

	if (load_config(argc, argv, &config) != 0)
	{
		fprintf(stderr, "Error: Unable to initialize the configuration.\n");
		return EXIT_FAILURE;
	}

	print_config(&config);

	while (running)
	{
		display_menu();

		if (read_menu_choice(&choice) != 0)
		{
			if (ferror(stdin))
			{
				fprintf(stderr, "Error: Unable to read menu input.\n");
				return EXIT_FAILURE;
			}

			if (feof(stdin))
			{
				printf("\nInput ended. Exiting the system.\n");
				break;
			}

			fprintf(stderr, "Invalid selection. Enter a number from 0 to 5.\n");
			continue;
		}

		switch (choice)
		{
		case MENU_EXIT:
			printf("Exiting the Warehouse Management System.\n");
			running = 0;
			break;

		case MENU_INVENTORY:
			printf("Inventory module is awaiting integration.\n");
			break;

		case MENU_ORDERS:
			printf("Orders module is awaiting integration.\n");
			break;

		case MENU_SEARCH:
			printf("Search module is awaiting integration.\n");
			break;

		case MENU_REPORTS:
			printf("Reports module is awaiting integration.\n");
			break;

		case MENU_ALERTS:
			printf("Alerts module is awaiting integration.\n");
			break;

		default:
			fprintf(stderr, "Error: Unexpected menu selection.\n");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: None.
 * Output: Writes all available menu choices to standard output.
 * Purpose: Display the Warehouse Management System menu.
 */
void display_menu(void)
{
	printf("\nWarehouse Management System\n");
	printf("1. Inventory\n");
	printf("2. Orders\n");
	printf("3. Search\n");
	printf("4. Reports\n");
	printf("5. Alerts\n");
	printf("0. Exit\n");
	printf("Enter your selection: ");
	fflush(stdout);
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: Editable menu text and a MenuChoice output pointer.
 * Output: Stores a validated choice and returns 0, otherwise returns -1.
 * Purpose: Convert validated character input into a menu selection.
 */
int parse_menu_choice(char* input, MenuChoice* out_choice)
{
	int converted_choice;

	if (util_check_null(input) != 0 || util_check_null(out_choice) != 0)
	{
		return -1;
	}

	if (util_validate_string(input, MENU_INPUT_CAPACITY - 1U) != 0)
	{
		return -1;
	}

	if (util_string_to_int(input, (int)MENU_EXIT, (int)MENU_ALERTS, &converted_choice) != 0)
	{
		return -1;
	}

	*out_choice = (MenuChoice)converted_choice;
	return 0;
}

/*
 * Author: Ivan Immanuel Shaji
 * Input: A MenuChoice output pointer.
 * Output: Reads and stores one validated selection, returning 0 or -1.
 * Purpose: Read one menu selection safely from standard input.
 */
int read_menu_choice(MenuChoice* out_choice)
{
	char input[MENU_INPUT_CAPACITY];
	size_t length;

	if (util_check_null(out_choice) != 0)
	{
		return -1;
	}

	if (fgets(input, (int)sizeof(input), stdin) == NULL)
	{
		return -1;
	}

	length = strlen(input);

	if (length > 0U && input[length - 1U] == '\n')
	{
		input[length - 1U] = '\0';
		--length;
	}
	else if (!feof(stdin))
	{
		discard_input_remainder();
		return -1;
	}

	if (length > 0U && input[length - 1U] == '\r')
	{
		input[length - 1U] = '\0';
	}

	return parse_menu_choice(input, out_choice);
}
