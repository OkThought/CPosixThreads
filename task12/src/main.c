#include "err_check.h"

#include <stdio.h>      // puts
#include <pthread.h>    // pthread_*
#include <stdlib.h>     // exit
#include <string.h>     // strerror

#define DEFAULT_ATTR NULL
#define NO_ARG NULL
#define NO_STATUS NULL
#define IGNORE_STATUS NULL

static const int COUNT_FROM = 1;
static const int COUNT_TO = 10;
static const int NAME_LENGTH = 8;
static const int SUCCESS = 0;

static pthread_mutexattr_t mutexattr;
static pthread_mutex_t mutex;
static pthread_cond_t cond;

enum Entity {
    CHILD, PARENT
};

static enum Entity printingEntity = PARENT;

void
PrintCount (enum Entity executingEntity, const char *name, int from, int to) {
    int count;
    const enum Entity waitingEntity = executingEntity == PARENT ? CHILD : PARENT;

    for (count = from; count <= to; ++count) {
        // EINVAL:       The value specified by mutex is invalid. (will not happen)
        // EDEADLK:     A deadlock would occur if the thread blocked waiting for mutex. (will not happen)
        (void) pthread_mutex_lock (&mutex);

        while (printingEntity != executingEntity) {
            // EINVAL:  The value specified by cond or the value specified by mutex is invalid. (will not happen)
            (void) pthread_cond_wait (&cond, &mutex);
        }

        (void) printf ("%*s counts %d\n", NAME_LENGTH, name, count);

        printingEntity = waitingEntity ;

        // EINVAL:  The value specified by mutex is invalid. (will not happen)
        // EPERM:   The current thread does not hold a lock on mutex. (will not happen)
        (void) pthread_mutex_unlock (&mutex);
    }
}

void*
RunChild (void *ignored) {
    PrintCount (CHILD, "Child", COUNT_FROM, COUNT_TO);
    pthread_exit (NO_STATUS);
}

int
InitializeResources () {
    int code;

    code = pthread_mutexattr_init (&mutexattr);
    if (code != SUCCESS) {
        fputs ("Couldn't init mutex attributes object\n", stderr);
        return code;
    };

    // EINVAL:   The value specified either by type or attr is invalid. (will not happen)
    (void) pthread_mutexattr_settype (&mutexattr, PTHREAD_MUTEX_ERRORCHECK);

    code = pthread_mutex_init (&mutex, &mutexattr);
    if (code != SUCCESS) {
        fputs ("Couldn't init mutex\n", stderr);
        return code;
    };

    code = pthread_cond_init (&cond, DEFAULT_ATTR);
    if (code != SUCCESS) {
        fputs ("Couldn't init cond\n", stderr);
        return code;
    };

    // EINVAL:  Invalid value for attr. (ignore)
    (void) pthread_mutexattr_destroy (&mutexattr);

    return SUCCESS;
}

int
DestroyResources () {
    int code;

    code = pthread_mutex_destroy (&mutex);
    if (code != SUCCESS)
        fprintf (stderr, "Couldn't destroy mutex: %s\n", strerror (code));

    code = pthread_cond_destroy (&cond);
    if (code != SUCCESS)
        fprintf (stderr, "Couldn't destroy cond: %s\n", strerror (code));

    return code;
}

int
main (int argc, char **argv) {
    pthread_t child_thread;
    int exit_status = EXIT_SUCCESS;
    int code;
    code = InitializeResources ();
    ExitIfNonZeroWithMessage (code, "Couldn't initialize resources");

    code = pthread_create (&child_thread, DEFAULT_ATTR, RunChild, NO_ARG);
    if (code != SUCCESS) {
        (void) DestroyResources ();
        fputs ("Couldn't start child_thread\n", stderr);
        exit (EXIT_FAILURE);
    };

    PrintCount (PARENT, "Parent", COUNT_FROM, COUNT_TO);

    // EINVAL:   The implementation has detected that the value specified by thread does not refer to a joinable thread.
    // (will not happen)
    // ESRCH:    No thread could be found corresponding to that specified by the given thread ID, thread.
    // (will not happen)
    // EDEADLK:  A deadlock was detected or the value of thread specifies the calling thread.
    // (will not happen)
    (void) pthread_join (child_thread, IGNORE_STATUS);

    code = DestroyResources ();
    if (code != SUCCESS) {
        exit_status = EXIT_FAILURE;
        fputs ("Error in DestroyResources\n", stderr);
    };

    exit (exit_status);
}
