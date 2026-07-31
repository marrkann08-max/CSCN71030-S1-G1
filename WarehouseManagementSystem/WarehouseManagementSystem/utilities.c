#include "utilities.h"

#include <ctype.h>
#include <string.h>

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