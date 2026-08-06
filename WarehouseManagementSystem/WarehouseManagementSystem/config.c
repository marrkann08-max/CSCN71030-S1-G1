//--------------------------------------------------------------------------
// config.c - Configuration Source file for the Warehouse Management System
//						Author: Ivan Immanuel Shaji
//--------------------------------------------------------------------------

#include "config.h"
#include <stdio.h>

/*
 * Input: Startup argument count, startup argument array, and an output pointer.
 * Output: Populates out_config and returns 0, or returns -1 for invalid input.
 * Purpose: Initialize the Config structure with safe default values.
 */
int load_config(int argc, char* argv[], Config* out_config)
{
    (void)argc;
    (void)argv;

    if (out_config == NULL)
    {
        return -1;
    }

    out_config->threshold = DEFAULT_THRESHOLD;
    out_config->seed = DEFAULT_SEED;
    return 0;
}

/*
 * Input: Read-only pointer to a populated Config structure.
 * Output: Displays the active threshold and seed values.
 * Purpose: Make the active configuration visible to the user.
 */
void print_config(const Config* config)
{
    if (config == NULL)
    {
        fprintf(stderr, "Unable to display a null Config.\n");
        return;
    }

    printf("Active configuration:\n");
    printf("Threshold: %d\n", config->threshold);
    printf("Seed: %u\n", config->seed);
}