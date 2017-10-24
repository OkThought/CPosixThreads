#include "err_check.h"

#include <stdio.h>          // printf
#include <pthread.h>        // pthread_*
#include <stdlib.h>         // exit
#include <unistd.h>         // sleep

#define SECONDS_TO_WAIT_BEFORE_CANCELLING 2
#define SECONDS_BETWEEN_MESSAGES 1
#define IGNORE_STATUS NULL
#define DEFAULT_ATTRS NULL
#define NO_ARG NULL

void *Run(void *);

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
    int count;
    for (count = 1; ; ++count) {
        pthread_testcancel();
        printf("Child counts \"%d\"\n", count);
        sleep(SECONDS_BETWEEN_MESSAGES);
    }
    pthread_exit(NULL);
}

/*
 * cancellation type (deferred, asynchronous)
 *
 * deferred means that cancellation can occur only at next cancellation point
 * asynchronous means that cancellation can occur at any time
 *
 * deferred properties:
 * - cancellation points are the only functions which respond to cancel requests under deferred cancelability
 * - the general rule is that all blocking functions are usually cancellation points
 * - when a thread reaches a cancellation point, the system checks for pending cancel message(s)
 *   - if a cancel is pending, the system will call all cleanup functions (see section below) & terminate the thread
 *   - if no cancel is pending, the function will proceed as normal
 *   - while the cancellation point is executing, if a cancellation requst comes in while the function is blocked, the
 *   wait will be interrupted and cleanup functions will be called
 *
 * asynchronous properties:
 * - useful to interrupt computation intensive operations
 * - avoid asynchronous cancelability at all costs -- it's dangerous
 * - if you must, be sure to call no code while asynchronous cancellation is enabled unless it's async-cancel safe
 * - while asynchronous cancellation is enabled, you cannot acquire any system resources
 * - Cleanup handlers are still called, but other functions you call (which are not async-cancel safe),
 * will not be able to clean up after themselves
 */
