#include "err_check.h"
#include "helpers.h"

#include <stdio.h>          // printf puts
#include <stdlib.h>         // exit
#include <math.h>           // M_PI

static const int NUMBER_OF_ITERATIONS = 200000000; // 2 * 10^8
static const int REQUIRED_NUMBER_OF_ARGUMENTS = 2;
static const int MAX_NUMBER_OF_THREADS = 100;

int main (int argc, char **argv) {
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

    Payload *payloads = ThreadPayloadsCreate (number_of_threads);
    ExitIfNullWithFormattedMessage ((void *) payloads, "Couldn't allocate %d payloads each of size %dB (total %dB)",
                                    number_of_threads, sizeof (Payload), sizeof (Payload) * number_of_threads);

    ThreadPayloadsInit (payloads, number_of_threads, NUMBER_OF_ITERATIONS);

    pthread_t threads[number_of_threads];
    code = StartParallelPiCalculation (threads, number_of_threads, payloads);
    ExitIfNonZeroWithCleanupAndMessage (code, ThreadPayloadsDelete, payloads,
                                        "Error on start parallel pi calculation");

    double pi;
    code = FinishParallelPiCalculation (threads, number_of_threads, &pi);
    ExitIfNonZeroWithCleanupAndMessage (code, ThreadPayloadsDelete, payloads,
                                        "Error on finish parallel pi calculation");

    ThreadPayloadsDelete (payloads);

    printf ("pi done - %.15g \n", pi);
    printf ("actual  - %.15g \n", M_PI);

    exit (EXIT_SUCCESS);
}

