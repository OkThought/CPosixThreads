#include "usage.h"

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

static const int SUCCESS = 0;

static int print_usage_header_v (const char *program_name, const char *program_description, int arguments_number,
                          va_list args) {
    fprintf (stderr, "Usage: %s", program_name);

    int i;
    for (i = 0; i < arguments_number; ++i) {
        int optional = va_arg (args, int);
        if (optional != OPTIONAL_ARGUMENT && optional != REQUIRED_ARGUMENT) {
            return EINVAL;
        }

        const char *arg_name = va_arg (args, const char *);
        if (arg_name == NULL) {
            return EINVAL;
        }

        va_arg (args, const char *); // arg_description

        if (optional) {
            fprintf (stderr, " [%s]", arg_name);
        } else {
            fprintf (stderr, " %s", arg_name);
        }
    }

    fputs ("\n\n", stderr);
    if (program_description != NO_DESCRIPTION) {
        fprintf (stderr, "\t%s\n\n", program_description);
    }

    return SUCCESS;
}

int
PrintUsage (const char *program_name, const char *program_description, int arguments_number, ...) {
    if (arguments_number < 0) {
        return EINVAL;
    }

    if (arguments_number == 0) {
        fprintf (stderr, "Usage: %s\n\n", program_name);
        if (program_description != NO_DESCRIPTION) {
            fprintf (stderr, "\t%s\n\n", program_description);
        }
        return SUCCESS;
    }
    va_list args;
    va_start (args, arguments_number);
    va_list args_copy;
    va_copy (args_copy, args);
    int code;
    code = print_usage_header_v (program_name, program_description, arguments_number, args_copy);
    va_end (args_copy);
    if (code != SUCCESS) {
        va_end (args);
        return code;
    }

    int i;
    for (i = 0; i < arguments_number; ++i) {
        int optional = va_arg (args, int);
        if (optional != OPTIONAL_ARGUMENT && optional != REQUIRED_ARGUMENT) {
            va_end (args);
            return EINVAL;
        }

        const char *arg_name = va_arg (args, const char *);
        if (arg_name == NULL) {
            va_end (args);
            return EINVAL;
        }

        const char *arg_description = va_arg (args, const char *);

        if (arg_description == NO_DESCRIPTION) {
            fprintf (stderr, "\t%s\n\n", arg_name);
        } else {
            fprintf (stderr, "\t%s - %s\n\n", arg_name, arg_description);
        }
    }

    va_end (args);

    return SUCCESS;
}
