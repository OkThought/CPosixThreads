#include "err_check.h"

#include <stdio.h>      // puts
#include <pthread.h>    // pthread_*
#include <stdlib.h>     // exit
#include <string.h>     // strerror
#include <errno.h>

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
static pthread_cond_t entity_switch_cond;

enum Entity {
    CHILD, PARENT
};

static enum Entity printingEntity = PARENT;

void
PrintCount (enum Entity executingEntity, const char *name, int from, int to) {
    int count;
    const enum Entity waitingEntity = executingEntity == PARENT ? CHILD : PARENT;

    for (count = from; count <= to; ++count) {
        (void) pthread_mutex_lock (&mutex);

        while (printingEntity != executingEntity) {
            (void) pthread_cond_wait (&entity_switch_cond, &mutex);
        }

        (void) printf ("%*s counts %d\n", NAME_LENGTH, name, count);

        printingEntity = waitingEntity ;

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
    if (code == ENOMEM) {
        (void) fputs ("Fatal: Not enough memory to init mutex attributes object\n", stderr);
        return code;
    }

    // ignore EINVAL
    (void) pthread_mutexattr_settype (&mutexattr, PTHREAD_MUTEX_ERRORCHECK);

    code = pthread_mutex_init (&mutex, &mutexattr);
    if (code == ENOMEM) {
        (void) fputs ("Fatal: Not enough memory to init mutex object\n", stderr);
        return code;
    } else if (code == EAGAIN) {
        (void) fputs ("Warning: System lacks resources to init mutex object\n", stderr);
        return code;
    } // ignore EPERM, EINVAL

    code = pthread_cond_init (&entity_switch_cond, DEFAULT_ATTR);
    if (code == ENOMEM) {
        (void) fputs ("Fatal: Not enough memory to init cond object\n", stderr);
        return code;
    } else if (code == EAGAIN) {
        (void) fputs ("Warning: System lacks resources to init cond object\n", stderr);
        return code;
    }

    (void) pthread_mutexattr_destroy (&mutexattr);

    return SUCCESS;
}

int
DestroyResources () {
    int code;

    code = pthread_mutex_destroy (&mutex);
    if (code != SUCCESS) {
        (void) fprintf (stderr, "Couldn't destroy mutex: %s\n", strerror (code));
    }

    code = pthread_cond_destroy (&entity_switch_cond);
    if (code != SUCCESS) {
        (void) fprintf (stderr, "Couldn't destroy cond: %s\n", strerror (code));
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
        (void) fputs ("Couldn't start child_thread\n", stderr);
        exit (EXIT_FAILURE);
    };

    PrintCount (PARENT, "Parent", COUNT_FROM, COUNT_TO);

    (void) pthread_join (child_thread, IGNORE_STATUS);

    code = DestroyResources ();
    if (code != SUCCESS) {
        exit_status = EXIT_FAILURE;
        (void) fputs ("Error in DestroyResources\n", stderr);
    };

    exit (exit_status);
}
