#include "authentication.h"

#include <ctype.h>
#include <stdio.h>
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

static int read_field(
    const char* prompt,
    char buffer[AUTH_FIELD_SIZE]
)
{
    size_t length;
    int character;

    fputs(prompt, stdout);

    if (fgets(buffer, (int)AUTH_FIELD_SIZE, stdin) == NULL)
    {
        return -1;
    }

    length = strlen(buffer);

    if (length > 0U && buffer[length - 1U] == '\n')
    {
        buffer[length - 1U] = '\0';
        return 0;
    }

    character = getchar();

    if (character == '\n' || character == EOF)
    {
        return 0;
    }

    do
    {
        character = getchar();
    }
    while (character != '\n' && character != EOF);

    buffer[0] = '\0';
    return -1;
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
    char username[AUTH_FIELD_SIZE];
    char normalized_username[AUTH_FIELD_SIZE];
    char password[AUTH_FIELD_SIZE];
    size_t attempt;
    size_t username_length;

    if (output_username == NULL || output_size == 0U)
    {
        return -1;
    }

    for (attempt = 0U; attempt < AUTH_MAX_ATTEMPTS; ++attempt)
    {
        if (read_field("Username: ", username) != 0 ||
            read_field("Password: ", password) != 0)
        {
            memset(password, 0, sizeof(password));
            continue;
        }

        if (authenticate(username, password) == 0 &&
            copy_trimmed_field(username, normalized_username) == 0)
        {
            username_length = strlen(normalized_username);

            if (username_length >= output_size)
            {
                memset(password, 0, sizeof(password));
                return -1;
            }

            memcpy(
                output_username,
                normalized_username,
                username_length + 1U
            );

            memset(password, 0, sizeof(password));
            return 0;
        }

        memset(password, 0, sizeof(password));
    }

    return -1;
}
