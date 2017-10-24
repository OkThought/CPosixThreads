#include "err_check.h"

#include <stdio.h>          // printf
#include <pthread.h>        // pthread_*
#include <stdlib.h>         // exit
#include <unistd.h>         // sleep

#define SECONDS_TO_WAIT_BEFORE_CANCELLING 3
#define SECONDS_BETWEEN_MESSAGES 1
#define IGNORE_STATUS NULL
#define DEFAULT_ATTRS NULL
#define NO_ARG NULL
#define EXECUTE_CLEANUP 0

void *Run(void *);
void Cleanup(void *);

int main() {
    pthread_t tid;

    int code = pthread_create(&tid, DEFAULT_ATTRS, Run, NO_ARG);
    ExitIfNonZeroWithMessage(code, "Error in pthread_create");

    sleep(SECONDS_TO_WAIT_BEFORE_CANCELLING);

    code = pthread_cancel(tid);
    ExitIfNonZeroWithMessage(code, "Error in pthread_cancel");

    code = pthread_join(tid, IGNORE_STATUS);
    ExitIfNonZeroWithMessage(code, "Error in pthread_join");

    exit(EXIT_SUCCESS);
}

void *Run(void *ignored) {
    pthread_cleanup_push(Cleanup, NULL);

    int count;
    for (count = 1; ; ++count) {
        printf("Child counts \"%d\"\n", count);
        sleep(SECONDS_BETWEEN_MESSAGES);
    }

    pthread_cleanup_pop(EXECUTE_CLEANUP);
    pthread_exit(NULL);
}

void Cleanup(void *ignored) {
    puts("I am cancelling..");
}
