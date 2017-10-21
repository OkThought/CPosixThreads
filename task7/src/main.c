#include "err_check.h"

#include <stdio.h>          // printf puts
#include <stdlib.h>         // strtol
#include <limits.h>         // strtol
#include <errno.h>          // errno
#include <pthread.h>        // pthread_*
#include <math.h>           // M_PI

#define ITERATIONS_NUMBER 200000000 // 2 * 10^8
#define DECIMAL_BASE 10

void *CalculatePI(void *);

typedef struct Payload {
    int start_index;        // initial  value for i in the loop (including)
    int finish_index;       // final    value for i in the loop (excluding)
} Payload;

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s number_of_threads\n", argv[0]);
        puts("\tnumber_of_threads - number of threads to run execution on");
        exit(EXIT_FAILURE);
    }

    errno = 0;
    long number_of_threads_long = strtol(argv[1], NULL, DECIMAL_BASE);
    ExitIfNonZeroWithFormattedMessage(errno, "Couldn't convert '%s' to int", argv[1]);

    ExitIfTrueWithErrcodeAndMessage(number_of_threads_long <= 0 || number_of_threads_long > INT_MAX, EINVAL,
                                    "number_of_threads must be positive and must fit into int size");

    int number_of_threads = (int) number_of_threads_long;

    pthread_t threads[number_of_threads];
    Payload *payloads = (Payload *) malloc(sizeof (Payload) * number_of_threads);
    ExitIfNullWithFormattedMessage((void *) payloads, "Couldn't allocate %d payloads each of size %dB (total %dB)",
                                   number_of_threads, sizeof (Payload), sizeof (Payload) * number_of_threads);

    int iterations_per_thread = ITERATIONS_NUMBER / number_of_threads;
    int iterations_rest = ITERATIONS_NUMBER - number_of_threads * iterations_per_thread;
    int prev_begin = 0;

    int code;
    int i;
    for (i = 0; i < number_of_threads; ++i) {
        payloads[i].start_index = prev_begin;
        prev_begin = payloads[i].finish_index = prev_begin + iterations_per_thread + (i < iterations_rest);

        code = pthread_create(threads + i, NULL, CalculatePI, (void *) (payloads + i));
        ExitIfNonZeroWithFormattedMessage(code, "Couldn't create thread #%d", i);
    }

    double pi = 0.;
    double *pi_part_ptr;
    void *status;

    for (i = 0; i < number_of_threads; ++i) {
        code = pthread_join(threads[i], &status);
        ExitIfNonZeroWithFormattedMessage(code, "Couldn't join thread #%d", i);

        pi_part_ptr = (double *) status;
        pi += *pi_part_ptr;
        free(pi_part_ptr);
    }


    pi *= 4.;
    printf("pi done - %.15g \n", pi);
    printf("actual  - %.15g \n", M_PI);

    exit (EXIT_SUCCESS);
}

void *CalculatePI(void *arg) {
    Payload *payload = (Payload *) arg;
    int i;
    double pi_part = 0;
    for (i = payload->start_index; i < payload->finish_index; i++) {
        pi_part += 1.0/(i*4.0 + 1.0);
        pi_part -= 1.0/(i*4.0 + 3.0);
    }

    double *pi_part_ptr = (double *) malloc(sizeof (double));
    ExitIfNullWithFormattedMessage(pi_part_ptr, "Couldn't allocate %dB for pi_part_ptr", sizeof (double));

    *pi_part_ptr = pi_part;
    pthread_exit((void *) pi_part_ptr);
}