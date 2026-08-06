//--------------------------------------------------------------------------
// config.h - Configuration Header file for the Warehouse Management System
//						Author: Ivan Immanuel Shaji
//--------------------------------------------------------------------------

#pragma once

#define DEFAULT_THRESHOLD 10
#define DEFAULT_SEED 1U
#define CONFIG_FILE_NAME "config.txt"

typedef struct Config
{
	int threshold;
	unsigned int seed;
} Config;

/*
* Input: Startup argument count, startup argument array, and an output pointer.
* Output: Populates out_config and returns 0, or returns -1 for invalid inputs.
* Purpose: Load defaults, file settings, and command-line overrides in order.
*/
int load_config(int argc, char* argv[], Config* out_config);

/*
* Input: ASCII configuration-file path and a Config output pointer.
* Output: Updates valid fields and returns 0, or returns -1 on any file error.
* Purpose: Read threshold and seed values from an ASCII configuration file.
*/
int read_config_file(const char* path, Config* out_config);

/*
* Input: Startup argument count, startup argument array, and a Config pointer.
* Output: Applies valid overrides and returns 0, or returns -1 for bad options.
* Purpose: Apply validated --threshold and --seed command-line overrides.
*/
int apply_cli_overrides(int argc, char* argv[], Config* out_config);

/*
* Input: Read-only pointer to a populated Config structure.
* Output: Displays the active threshold and seed values.
* Purpose: Make the active configuration visible to the user.
*/
void print_config(const Config* config);