#include "err_check.h"

#include <stdio.h>      // puts
#include <pthread.h>    // pthread_*
#include <stdlib.h>     // exit
#include <string.h>     // strerror
#include <unistd.h>     // usleep

#define DEFAULT_ATTR NULL
#define NO_ARG NULL
#define NO_STATUS NULL
#define IGNORE_STATUS NULL
#define MUTEXES_NUMBER 3

static const int COUNT_FROM = 1;
static const int COUNT_TO = 10;
static const int NAME_LENGTH = 8;
static const int SUCCESS = 0;
static const int TIME_WAIT_BEFORE_CHILD_RUN = 100000; // 100 millis
static const int PRINTING_PERIOD = 500000; // 500 millis
static const int CHILD_INITIAL_MUTEX_ID = 2;
static const int PARENT_INITIAL_MUTEX_ID = 0;

static pthread_mutexattr_t mutexattr;
static pthread_mutex_t global_mutexes[MUTEXES_NUMBER];

int
LockMutexByCycledId (const char *locking_entity_name, unsigned mutex_id) {
    mutex_id %= MUTEXES_NUMBER;
    int code = pthread_mutex_lock (&global_mutexes[mutex_id]);
//    printf ("%s locks mutex %d\n", locking_entity_name, mutex_id);
    return code;
}

int
UnlockMutexByCycledId (const char *locking_entity_name, unsigned mutex_id) {
    mutex_id %= MUTEXES_NUMBER;
    int code = pthread_mutex_unlock (&global_mutexes[mutex_id]);
//    printf ("%s unlocks mutex %d\n", locking_entity_name, mutex_id);
    return code;
}

void
PrintCount (unsigned initial_mutex_id, const char *name, int from, int to) {
    int count;
    unsigned mutex_id = initial_mutex_id;
    for (count = from; count <= to; ++count) {
        LockMutexByCycledId (name, mutex_id+1);
        (void) printf ("%*s counts %d\n", NAME_LENGTH, name, count);
        usleep (PRINTING_PERIOD);
        UnlockMutexByCycledId (name, mutex_id);
        ++mutex_id;
    }
    UnlockMutexByCycledId (name, mutex_id);

}

void*
RunChild (void *ignored) {
    LockMutexByCycledId ("Child", CHILD_INITIAL_MUTEX_ID);
    usleep (TIME_WAIT_BEFORE_CHILD_RUN);
    PrintCount (CHILD_INITIAL_MUTEX_ID, "Child", COUNT_FROM, COUNT_TO);
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

    int i;
    for (i = 0; i < MUTEXES_NUMBER; ++i) {
        code = pthread_mutex_init (&global_mutexes[i], &mutexattr);
        if (code != SUCCESS) {
            fputs ("Couldn't init mutex\n", stderr);
            return code;
        };
    }

    // EINVAL:  Invalid value for attr. (ignore)
    (void) pthread_mutexattr_destroy (&mutexattr);

    return SUCCESS;
}

int
DestroyResources () {
    int code = SUCCESS;
    int i;
    for (i = 0; i < MUTEXES_NUMBER; ++i) {
        code = pthread_mutex_destroy (&global_mutexes[i]);
        if (code != SUCCESS) {
            fprintf (stderr, "Couldn't destroy mutex: %s\n", strerror (code));
        }
    }

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

    LockMutexByCycledId ("Parent", PARENT_INITIAL_MUTEX_ID);

    PrintCount (PARENT_INITIAL_MUTEX_ID, "Parent", COUNT_FROM, COUNT_TO);

    (void) pthread_join (child_thread, IGNORE_STATUS);

    code = DestroyResources ();
    if (code != SUCCESS) {
        exit_status = EXIT_FAILURE;
        fputs ("Error in DestroyResources\n", stderr);
    };

    exit (exit_status);
}
