#ifndef UTILITIES_H
#define UTILITIES_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Validates, trims, and limits a string.
 *
 * Author: Keshav Kumar Markan
 * Input: text is an editable string and max_length is the maximum number of
 * characters that may remain in the string.
 * Output: text contains the trimmed, length-limited, null-terminated result.
 * Purpose: Provide safe, shared string validation for other project modules.
 *
 * @param text String to validate and modify.
 * @param max_length Maximum permitted number of characters, excluding '\0'.
 * @return 0 on success, or -1 when the input is invalid.
 */
int util_validate_string(char *text, size_t max_length);

/**
 * @brief Converts text to a signed integer within a required range.
 *
 * Author: Keshav Kumar Markan
 * Input: text, minimum, maximum, and a valid output pointer.
 * Output: out_value receives the converted integer only on success.
 * Purpose: Safely convert complete signed-integer input for calling modules.
 *
 * @param text Text containing a signed integer.
 * @param minimum Smallest permitted value.
 * @param maximum Largest permitted value.
 * @param out_value Destination for the converted value.
 * @return 0 on success, or -1 when conversion or range validation fails.
 */
int util_string_to_int(
    const char *text,
    int minimum,
    int maximum,
    int *out_value
);

/**
 * @brief Converts text to an unsigned integer within a required range.
 *
 * Author: Keshav Kumar Markan
 * Input: text, minimum, maximum, and a valid output pointer.
 * Output: out_value receives the converted unsigned integer only on success.
 * Purpose: Safely convert complete unsigned-integer input for calling modules.
 *
 * @param text Text containing an unsigned integer.
 * @param minimum Smallest permitted value.
 * @param maximum Largest permitted value.
 * @param out_value Destination for the converted value.
 * @return 0 on success, or -1 when the input is negative, invalid, or outside
 * the permitted range.
 */
int util_string_to_uint(
    const char *text,
    unsigned int minimum,
    unsigned int maximum,
    unsigned int *out_value
);

/**
 * @brief Checks whether a pointer is valid before it is dereferenced.
 *
 * Author: Keshav Kumar Markan
 * Input: pointer is the address to check.
 * Output: No output parameter.
 * Purpose: Provide a consistent null-pointer check to calling modules.
 *
 * @param pointer Pointer to check.
 * @return 0 when the pointer is not NULL, or -1 when it is NULL.
 */
int util_check_null(const void *pointer);

#ifdef __cplusplus
}
#endif

#endif
