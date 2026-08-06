//--------------------------------------------------------------------------
// main.c - Entry point for the Warehouse Management System
//                    Author: Ivan Immanuel Shaji
//--------------------------------------------------------------------------

#include "main.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * Author: Ivan Immanuel Shaji
 * Input: Startup argument count and startup argument array.
 * Output: Returns EXIT_SUCCESS after successful startup, otherwise EXIT_FAILURE.
 * Purpose: Load the runtime configuration and start the warehouse system.
 */
int main(int argc, char* argv[])
{
	Config config;

	if (load_config(argc, argv, &config) != 0)
	{
		fprintf(stderr, "Error: Unable to initialize the configuration.\n");
		return EXIT_FAILURE;
	}

	print_config(&config);
	display_menu();

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
}