#include "parse.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define SUCCESS 0
#define DECIMAL_BASE 10

int
ParseInt (int *value_ptr, const char *value_name, const char *value_string, int min_value, int max_value) {
    char *first_invalid_char_ptr;
    long long_value = strtol (value_string, &first_invalid_char_ptr, DECIMAL_BASE);

    if (*first_invalid_char_ptr != '\0' || first_invalid_char_ptr == value_string) {
        fprintf (stderr, "Couldn't parse %s from '%s'\n", value_name, value_string);
        return EINVAL;
    }

    if (long_value < min_value || long_value > max_value) {
        fprintf (stderr, "%s not in required range %d...%d\n", value_name, min_value, max_value);
        return ERANGE;
    }

    *value_ptr = (int) long_value;

    return SUCCESS;
}
