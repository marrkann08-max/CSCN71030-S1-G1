//--------------------------------------------------------------------------
// config.c - Configuration Source file for the Warehouse Management System
//						Author: Ivan Immanuel Shaji
//--------------------------------------------------------------------------

#include "config.h"
#include "utilities.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#define CONFIG_LINE_CAPACITY 256U

/*
 * Input: Config pointer plus one setting name and value.
 * Output: Stores a validated recognized value and returns 0, otherwise -1.
 * Purpose: Validate file settings before changing the Config structure.
 */
static int apply_setting(Config* config, const char* name, const char* value)
{
	int threshold;
	unsigned int seed;

	if (util_check_null(config) != 0 || util_check_null(name) != 0 || util_check_null(value) != 0)
	{
		return -1;
	}

	if (strcmp(name, "threshold") == 0)
	{
		if (util_string_to_int(value, 1, INT_MAX, &threshold) != 0)
		{
			return -1;
		}

		config->threshold = threshold;
		return 0;
	}

	if (strcmp(name, "seed") == 0)
	{
		if (util_string_to_uint(value, 1U, UINT_MAX, &seed) != 0)
		{
			return -1;
		}

		config->seed = seed;
		return 0;
	}

	return -1;
}

/*
 * Input: An open ASCII stream positioned inside an overlength line.
 * Output: Advances the stream to the next line or end of file.
 * Purpose: Recover safely when one line is longer than the input buffer.
 */
static void discard_line_remainder(FILE* file)
{
	int character;

	if (file == NULL)
	{
		return;
	}

	do
	{
		character = fgetc(file);
	} while (character != '\n' && character != EOF);
}

/*
 * Input: Startup argument count, startup argument array, and an output pointer.
 * Output: Populates out_config and returns 0, or returns -1 for invalid input.
 * Purpose: Initialize defaults and then read optional file-based settings.
 */
int load_config(int argc, char* argv[], Config* out_config)
{
	Config loaded_config;

	(void)argc;
	(void)argv;

	if (util_check_null(out_config) != 0)
	{
		return -1;
	}

	loaded_config.threshold = DEFAULT_THRESHOLD;
	loaded_config.seed = DEFAULT_SEED;

	if (read_config_file(CONFIG_FILE_NAME, &loaded_config) != 0)
	{
		fprintf(stderr, "Warning: Config file could not be read; defaults were kept.\n");
	}

	*out_config = loaded_config;
	return 0;
}

/*
 * Input: ASCII configuration-file path and a Config output pointer.
 * Output: Updates valid fields and returns 0, or returns -1 on a file error.
 * Purpose: Read threshold and seed values from an ASCII configuration file.
 */
int read_config_file(const char* path, Config* out_config)
{
	FILE* file = NULL;
	char line[CONFIG_LINE_CAPACITY];
	char* separator;
	char* name;
	char* value;
	size_t length;
	int result = 0;

	if (util_check_null(path) != 0 || util_check_null(out_config) != 0)
	{
		return -1;
	}

	if (fopen_s(&file, path, "r") != 0 || file == NULL)
	{
		return -1;
	}

	while (fgets(line, (int)sizeof(line), file) != NULL)
	{
		length = strlen(line);

		if (length > 0U && line[length - 1U] == '\n')
		{
			line[length - 1U] = '\0';
		}
		else if (!feof(file))
		{
			discard_line_remainder(file);
			fprintf(stderr, "Warning: Config line is too long.\n");
			continue;
		}

		length = strlen(line);
		if (length > 0U && line[length - 1U] == '\r')
		{
			line[length - 1U] = '\0';
		}

		name = line;
		if (util_validate_string(name, sizeof(line) - 1U) != 0)
		{
			continue;
		}

		if (name[0] == '#')
		{
			continue;
		}

		separator = strchr(name, '=');
		if (separator == NULL)
		{
			fprintf(stderr, "Warning: Invalid Config line: %s\n", name);
			continue;
		}

		*separator = '\0';
		value = separator + 1;

		if (util_validate_string(name, sizeof(line) - 1U) != 0 || util_validate_string(value, sizeof(line) - 1U) != 0 || apply_setting(out_config, name, value) != 0)
		{
			fprintf(stderr, "Warning: Invalid Config setting.\n");
		}
	}

	if (ferror(file) != 0)
	{
		result = -1;
	}

	if (fclose(file) != 0)
	{
		result = -1;
	}

	return result;
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
