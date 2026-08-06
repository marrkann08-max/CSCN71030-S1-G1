#include "utilities.h"

#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

int util_validate_string(char* text, size_t max_length)
{
    size_t original_length;
    size_t start;
    size_t end;
    size_t stored_length;
    size_t index;

    if (text == NULL || max_length == 0U)
    {
        return -1;
    }

    original_length = strlen(text);
    start = 0U;

    /* Locate the first non-whitespace character. */
    while (start < original_length &&
        isspace((unsigned char)text[start]))
    {
        ++start;
    }

    /* Locate the position after the last non-whitespace character. */
    end = original_length;

    while (end > start &&
        isspace((unsigned char)text[end - 1U]))
    {
        --end;
    }

    /* Reject empty and whitespace-only strings. */
    if (start == end)
    {
        return -1;
    }

    /* Reject embedded control characters such as tabs. */
    for (index = start; index < end; ++index)
    {
        if (iscntrl((unsigned char)text[index]))
        {
            return -1;
        }
    }

    stored_length = end - start;

    /* Limit overlength strings to the permitted maximum. */
    if (stored_length > max_length)
    {
        stored_length = max_length;
    }

    memmove(text, text + start, stored_length);
    text[stored_length] = '\0';

    return 0;
}

int util_check_null(const void* pointer)
{
    if (pointer == NULL)
    {
        return -1;
    }

    return 0;
}

int util_string_to_int(
    const char* text,
    int minimum,
    int maximum,
    int* out_value
)
{
    char* conversion_end;
    long converted_value;

    if (text == NULL || out_value == NULL || minimum > maximum)
    {
        return -1;
    }

    errno = 0;
    converted_value = strtol(text, &conversion_end, 10);

    if (conversion_end == text || errno == ERANGE)
    {
        return -1;
    }

    /* Allow trailing whitespace, but reject other trailing characters. */
    while (*conversion_end != '\0' &&
        isspace((unsigned char)*conversion_end))
    {
        ++conversion_end;
    }

    if (*conversion_end != '\0')
    {
        return -1;
    }

    if (converted_value < minimum || converted_value > maximum)
    {
        return -1;
    }

    *out_value = (int)converted_value;

    return 0;
}

int util_string_to_uint(
    const char* text,
    unsigned int minimum,
    unsigned int maximum,
    unsigned int* out_value
)
{
    const char* first_character;
    char* conversion_end;
    unsigned long converted_value;

    if (text == NULL || out_value == NULL || minimum > maximum)
    {
        return -1;
    }

    first_character = text;

    while (isspace((unsigned char)*first_character))
    {
        ++first_character;
    }

    /* strtoul can accept a minus sign, so reject it explicitly. */
    if (*first_character == '-')
    {
        return -1;
    }

    errno = 0;
    converted_value = strtoul(text, &conversion_end, 10);

    if (conversion_end == text ||
        errno == ERANGE ||
        converted_value > UINT_MAX)
    {
        return -1;
    }

    while (*conversion_end != '\0' &&
        isspace((unsigned char)*conversion_end))
    {
        ++conversion_end;
    }

    if (*conversion_end != '\0')
    {
        return -1;
    }

    if (converted_value < minimum || converted_value > maximum)
    {
        return -1;
    }

    *out_value = (unsigned int)converted_value;

    return 0;
}