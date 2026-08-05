#include "authentication.h"

#include <ctype.h>
#include <string.h>

static const char credentials[AUTH_MAX_USERS][2][AUTH_FIELD_SIZE] =
{
    { "admin", "Admin123" },
    { "manager", "Manager123" },
    { "warehouse", "Warehouse123" }
};

static int copy_trimmed_field(
    const char* source,
    char destination[AUTH_FIELD_SIZE]
)
{
    size_t begin = 0U;
    size_t end;
    size_t length = 0U;

    if (source == NULL)
    {
        return -1;
    }

    while (length < AUTH_FIELD_SIZE && source[length] != '\0')
    {
        ++length;
    }

    if (length == 0U || length == AUTH_FIELD_SIZE)
    {
        return -1;
    }

    while (begin < length && isspace((unsigned char)source[begin]))
    {
        ++begin;
    }

    end = length;
    while (end > begin && isspace((unsigned char)source[end - 1U]))
    {
        --end;
    }

    if (begin == end)
    {
        return -1;
    }

    memcpy(destination, source + begin, end - begin);
    destination[end - begin] = '\0';

    return 0;
}

int authenticate(
    const char* username,
    const char* password
)
{
    char username_copy[AUTH_FIELD_SIZE];
    char password_copy[AUTH_FIELD_SIZE];
    size_t index;

    if (copy_trimmed_field(username, username_copy) != 0 ||
        copy_trimmed_field(password, password_copy) != 0)
    {
        return -1;
    }

    for (index = 0U; index < AUTH_MAX_USERS; ++index)
    {
        if (strcmp(username_copy, credentials[index][0]) == 0 &&
            strcmp(password_copy, credentials[index][1]) == 0)
        {
            memset(password_copy, 0, sizeof(password_copy));
            return 0;
        }
    }

    memset(password_copy, 0, sizeof(password_copy));
    return -1;
}

int authentication_login(
    char* output_username,
    size_t output_size
)
{
    (void)output_username;
    (void)output_size;

    return -1;
}
