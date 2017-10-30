#include "err_check.h"

#include <stdio.h>      // puts
#include <pthread.h>    // pthread_*

#define NUMBER_OF_LINES 10
#define DEFAULT_ATTR NULL
#define NO_ARG NULL
#define NO_STATUS NULL

void PrintLines(char *line, int line_number) {
    int i;
    for (i = 0; i < line_number; ++i) {
        puts(line);
    }
}

void* RunBill(void *ignored) {
    PrintLines("Hello! I'm Bill!", NUMBER_OF_LINES);
    pthread_exit(NO_STATUS);
}

int main(int argc, char **argv) {
    pthread_t bill;

    int code = pthread_create(&bill, DEFAULT_ATTR, RunBill, NO_ARG);

    ExitIfNonZeroWithMessage(code, "Couldn't start Bill's thread");

    PrintLines("Hello! I'm Rob!", NUMBER_OF_LINES);

    pthread_exit(NO_STATUS);
}
