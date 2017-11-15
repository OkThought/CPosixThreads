#ifndef UTIL_USAGE_H
#define UTIL_USAGE_H

const int OPTIONAL_ARGUMENT = 1;
const int REQUIRED_ARGUMENT = 0;
const char *NO_DESCRIPTION = NULL;

int        PrintUsage (const char *program_name, const char *program_description, int arguments_number, ...);

#endif //UTIL_USAGE_H
