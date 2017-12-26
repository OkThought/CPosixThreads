#include "err_check.h"

#include <stdio.h>      // puts
#include <pthread.h>    // pthread_*
#include <stdlib.h>     // exit
#include <errno.h>
#include <semaphore.h>
#include <string.h>

#define DEFAULT_ATTR NULL
#define NO_ARG NULL
#define NO_STATUS NULL
#define IGNORE_STATUS NULL
#define NOT_PSHARED 0

static const int COUNT_FROM = 1;
static const int COUNT_TO = 10;
static const int NAME_LENGTH = 8;
static const int SUCCESS = 0;

#define SEMAPHORE_NUMBER 2
static sem_t semaphores[SEMAPHORE_NUMBER];

enum Entity {
    CHILD = 0,
    PARENT = 1,
};

int
InitializeResources () {
    int code = SUCCESS;

    int i;
    for (i = 0; i < SEMAPHORE_NUMBER; ++i) {
#ifndef __APPLE__
        code = sem_init (&semaphores[i], NOT_PSHARED, i);
#endif
        if (code == ENOSPC) {
            (void) fprintf (stderr, "A resource required to initialize the semaphore has been exhausted, "
                    "or the limit on semaphores ({SEM_NSEMS_MAX}) has been reached.");
            return code;
        }
    }

    return SUCCESS;
}

int
DestroyResources () {
    int i;
    for (i = 0; i < SEMAPHORE_NUMBER; ++i) {
#ifndef __APPLE__
        (void) sem_destroy (&semaphores[i]);
#endif
    }

    return SUCCESS;
}

void
PrintCount (enum Entity executingEntity, const char *name, int from, int to) {
    int count;
    const enum Entity waitingEntity = executingEntity == PARENT ? CHILD : PARENT;

    int code;
    for (count = from; count <= to; ++count) {
        do {
            code = sem_wait (&semaphores[executingEntity]);
        } while (code == EINTR);
        ExitIfNonZero (code);

        (void) printf ("%*s counts %d\n", NAME_LENGTH, name, count);

        code = sem_post (&semaphores[waitingEntity]);
        ExitIfNonZeroWithMessage (code, strerror(errno));
    }
}

void*
RunChild (void *ignored) {
    PrintCount (CHILD, "Child", COUNT_FROM, COUNT_TO);
    pthread_exit (NO_STATUS);
}

int
main (int argc, char **argv) {
    pthread_t child_thread;
    int exit_status = EXIT_SUCCESS;
    int code;
    code = InitializeResources ();
    ExitIfNonZeroWithMessage (code, "Couldn't initialize resources");

    code = pthread_create (&child_thread, DEFAULT_ATTR, RunChild, NO_ARG);
    ExitIfNonZeroWithMessage (code, "Couldn't start child");

    PrintCount (PARENT, "Parent", COUNT_FROM, COUNT_TO);

    (void) pthread_join (child_thread, IGNORE_STATUS);

    code = DestroyResources ();
    if (code != SUCCESS) {
        exit_status = EXIT_FAILURE;
        (void) fputs ("Error in DestroyResources\n", stderr);
    };

    exit (exit_status);
}
