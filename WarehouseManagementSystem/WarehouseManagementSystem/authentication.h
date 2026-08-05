#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUTH_MAX_USERS 3U
#define AUTH_FIELD_SIZE 64U
#define AUTH_MAX_ATTEMPTS 3U

    int authenticate(
        const char* username,
        const char* password
    );

    int authentication_login(
        char* output_username,
        size_t output_size
    );

#ifdef __cplusplus
}
#endif

#endif