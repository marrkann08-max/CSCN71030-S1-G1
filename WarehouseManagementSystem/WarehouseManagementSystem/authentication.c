#include "authentication.h"

#include <string.h>

static const char credentials[AUTH_MAX_USERS][2][AUTH_FIELD_SIZE] =
{
    { "admin", "Admin123" },
    { "manager", "Manager123" },
    { "warehouse", "Warehouse123" }
};

int authenticate(
    const char* username,
    const char* password
)
{
    size_t index;

    for (index = 0U; index < AUTH_MAX_USERS; ++index)
    {
        if (strcmp(username, credentials[index][0]) == 0 &&
            strcmp(password, credentials[index][1]) == 0)
        {
            return 0;
        }
    }

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
