#include "err_check.h"

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>

#define SECONDS_TO_WAIT_BEFORE_CANCELLING 1
#define SECONDS_TO_WAIT_TO_BE_CANCELLED 2

void *run(void *);

int main() {
    pthread_t tid;
    int code = pthread_create(&tid, NULL, run, NULL);
    ExitIfNonZeroWithMessage(code, "pthread_create");

    sleep(SECONDS_TO_WAIT_BEFORE_CANCELLING);

    code = pthread_cancel(tid);
    ExitIfNonZeroWithMessage(code, "pthread_cancel");

    code = pthread_join(tid, NULL);
    ExitIfNonZeroWithMessage(code, "pthread_join");

    exit(EXIT_SUCCESS);
}

void *run(void *ignored) {
    pthread_testcancel();
    printf("Hello from child thread!\n");
    sleep(SECONDS_TO_WAIT_TO_BE_CANCELLED);
    pthread_exit(NULL);
}
