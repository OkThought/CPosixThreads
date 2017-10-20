#include "err_check.h"

#include <stdio.h>      // puts
#include <pthread.h>    // pthread_*

#define NUMBER_OF_LINES 10
#define DEFAULT_ATTRS NULL
#define NO_ARG NULL
#define NO_STATUS NULL
#define BILL_EXIT_STATUS (void *) 42

void PrintLines(char *line, int line_number) {
    int i;
    for (i = 0; i < line_number; ++i) {
        puts(line);
    }
}

void* RunBill(void *ignored) {
    PrintLines("Hello! I'm Bill!", NUMBER_OF_LINES);
    pthread_exit(BILL_EXIT_STATUS);
}

int main(int argc, char **argv) {
    pthread_t bill;

    int code = pthread_create(&bill, DEFAULT_ATTRS, RunBill, NO_ARG);

    ExitIfNonZeroWithMessage(code, "Couldn't start Bill's thread");

    void *status;
    code = pthread_join(bill, &status);
    ExitIfNonZeroWithMessage(code, "Problem joining Bill's thread occurred");

    printf("Bill finished running with exit code: %d\n", (int) status);

    PrintLines("Hello! I'm Rob!", NUMBER_OF_LINES);

    return 0;
}
