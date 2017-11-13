#include "err_check.h"
#include "helpers.h"

#include <stdio.h>          // printf puts
#include <stdlib.h>         // exit
#include <math.h>           // M_PI

static const int NUMBER_OF_ITERATIONS_PER_CHUNK = 1024;
static const int REQUIRED_NUMBER_OF_ARGUMENTS = 2;

int
main (int argc, char **argv) {
    if (argc != REQUIRED_NUMBER_OF_ARGUMENTS) {
        PrintUsage ();
        exit (EXIT_FAILURE);
    }

    int number_of_threads;
    int code;
    char *number_of_threads_string = argv[1];
    code = ParseNumberOfThreads (number_of_threads_string, &number_of_threads, MAX_NUMBER_OF_THREADS);
    ExitIfNonZeroWithFormattedMessage (code, "Couldn't parse number of threads from string '%s'",
                                       number_of_threads_string);

    PiCalcTask *tasks = PiCalcTasksCreate (number_of_threads);
    ExitIfNullWithFormattedMessage ((void *) tasks, "Couldn't create %d tasks", number_of_threads);

    PiCalcTasksInit (tasks, number_of_threads);

    pthread_t threads[number_of_threads];
    code = StartParallelPiCalculation (threads, number_of_threads, tasks, NUMBER_OF_ITERATIONS_PER_CHUNK);
    ExitIfNonZeroWithCleanupAndMessage (code, PiCalcTasksDelete, tasks,
                                        "Error on start parallel pi calculation");

    double pi;
    code = FinishParallelPiCalculation (threads, number_of_threads, &pi);
    ExitIfNonZeroWithCleanupAndMessage (code, PiCalcTasksDelete, tasks,
                                        "Error on finish parallel pi calculation");

    PiCalcTasksDelete (tasks);

    printf ("pi done - %.15g \n", pi);
    printf ("actual  - %.15g \n", M_PI);

    exit (EXIT_SUCCESS);
}

