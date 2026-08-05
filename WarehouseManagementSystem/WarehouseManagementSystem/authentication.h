#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUTH_MAX_USERS 3U
#define AUTH_FIELD_SIZE 64U
#define AUTH_MAX_ATTEMPTS 3U

    /*
     * Author: Keshav Kumar Markan
     * Input: Username and password strings.
     * Output: Returns 0 for valid credentials and -1 otherwise.
     * Purpose: Validate credentials without reading interactive input.
     */
    int authenticate(
        const char* username,
        const char* password
    );

    /*
     * Author: Keshav Kumar Markan
     * Input: Writable username output buffer and its capacity.
     * Output: Stores the authenticated username and returns 0, or returns -1.
     * Purpose: Allow at most three interactive authentication attempts.
     */
    int authentication_login(
        char* output_username,
        size_t output_size
    );

#ifdef __cplusplus
}
#endif

#endif
