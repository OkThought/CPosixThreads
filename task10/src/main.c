#include "err_check.h"

#include <stdio.h>      // puts
#include <pthread.h>    // pthread_*
#include <unistd.h>     // sleep
#include <stdlib.h>     // exit
#include <string.h>     // strerror

#define DEFAULT_ATTRS NULL
#define NO_ARG NULL
#define NO_STATUS NULL
#define IGNORE_STATUS NULL
#define TRUE 1
#define FALSE 0

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

int PrintCount (enum Entity executingEntity, const char *name, int from, int to) {
    int count;
    int code;
    const enum Entity waitingEntity = executingEntity == PARENT ? CHILD : PARENT;

    for (count = from; count <= to; ++count) {
        code = pthread_mutex_lock (&mutex);
        if (code != SUCCESS) {
            fprintf (stderr, "Couldn't lock the mutex in %s: %s\n", name, strerror(code));
            return code;
        }

        while (printingEntity != executingEntity) {
            code = pthread_cond_wait (&cond, &mutex);
            if (code != SUCCESS) {
                fprintf (stderr, "Couldn't wait for cond on mutex in %s: %s\n", name, strerror(code));
                return code;
            }
        }

        printf ("%*s counts %d\n", NAME_LENGTH, name, count);

        printingEntity = waitingEntity ;

        code = pthread_mutex_unlock (&mutex);
        if (code != SUCCESS) {
            fprintf (stderr, "Couldn't unlock the mutex in %s: %s\n", name, strerror(code));
            return code;
        }
    }

    return SUCCESS;
}

void* RunChild (void *ignored) {
    PrintCount (CHILD, "Child", COUNT_FROM, COUNT_TO);
    pthread_exit (NO_STATUS);
}

int initializeResources () {
    int code;

    code = pthread_mutexattr_init (&mutexattr);
    if (code != SUCCESS) {
        fputs("Couldn't init mutex attributes object\n", stderr);
        return code;
    };

    code = pthread_mutexattr_settype (&mutexattr, PTHREAD_MUTEX_ERRORCHECK);
    if (code != SUCCESS) {
        fputs("Couldn't set mutex type to ERRORCHECK\n", stderr);
        return code;
    };

    code = pthread_mutex_init (&mutex, &mutexattr);
    if (code != SUCCESS) {
        fputs("Couldn't init mutex\n", stderr);
        return code;
    };

    code = pthread_cond_init (&cond, DEFAULT_ATTRS);
    if (code != SUCCESS) {
        fputs("Couldn't init cond\n", stderr);
        return code;
    };

    code = pthread_mutexattr_destroy (&mutexattr);
    if (code != SUCCESS) {
        fputs("Couldn't destroy mutex attributes object\n", stderr);
        return code;
    };

    return SUCCESS;
}

int destroyResources() {
    int code;

    code = pthread_mutex_destroy (&mutex);
    if (code != SUCCESS)
        fprintf (stderr, "Couldn't destroy mutex: %s\n", strerror(code));

    code = pthread_cond_destroy (&cond);
    if (code != SUCCESS)
        fprintf (stderr, "Couldn't destroy cond: %s\n", strerror(code));

    return code;
}

int main(int argc, char **argv) {
    pthread_t child_thread;
    int code;
    int exit_status = EXIT_SUCCESS;

    code = initializeResources ();
    ExitIfNonZeroWithMessage (code, "Couldn't initialize resources");

    code = pthread_create (&child_thread, DEFAULT_ATTRS, RunChild, NO_ARG);
    if (code != SUCCESS) {
        (void) destroyResources ();
        exit_status = EXIT_FAILURE;
        fputs("Couldn't start child_thread\n", stderr);
    };

    code = PrintCount (PARENT, "Parent", COUNT_FROM, COUNT_TO);
    if (code != SUCCESS) {
        (void) destroyResources ();
        exit_status = EXIT_FAILURE;
        fputs("Error in PrintCount\n", stderr);
    };

    code = pthread_join(child_thread, IGNORE_STATUS);
    if (code != SUCCESS) {
        (void) destroyResources ();
        exit_status = EXIT_FAILURE;
        fputs("Couldn't join child_thread\n", stderr);
    };

    code = destroyResources ();
    if (code != SUCCESS) {
        exit_status = EXIT_FAILURE;
        fputs("Error in destroyResources\n", stderr);
    };

    exit (exit_status);
}
