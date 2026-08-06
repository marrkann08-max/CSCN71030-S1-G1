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

	return EXIT_SUCCESS;
}