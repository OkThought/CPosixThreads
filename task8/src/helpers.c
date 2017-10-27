#include "helpers.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <helpers.h>
#include <signal.h>

#define FALSE 0
#define TRUE 1

struct Payload {
    int start;              // initial  value for i in the loop (including)
    int step;               // step with which to iterate through the loop
    int chunk;              // number of iterations per chunk
    double pi_part;         // partial sum
};

int should_stop = FALSE;

// private
void
interrupt_handler (int sig) {
    puts ("\rStopping");
    should_stop = TRUE;
}

void
PrintUsage () {
    fputs("Usage:\t<program_name> <number_of_threads>\n", stderr);
}

int
ParseNumberOfThreads (const char *number_of_threads_string, int *number_of_threads, int max) {
    if (number_of_threads_string == NULL)
        return EINVAL;

    errno = 0;
    static const int DECIMAL_BASE = 10;
    char *first_invalid_char;
    long number_of_threads_long = strtol (number_of_threads_string, &first_invalid_char, DECIMAL_BASE);
    if (errno != 0 || first_invalid_char == number_of_threads_string) {
        fprintf (stderr, "Couldn't parse int '%s' with base %d\n", number_of_threads_string, DECIMAL_BASE);
        return errno;
    };

    if (*first_invalid_char != '\0') {
        fprintf (stderr, "'%s' is not a number\n", number_of_threads_string);
        return EINVAL;
    }

    if (number_of_threads_long <= 0) {
        fputs ("number_of_threads is not positive\n", stderr);
        return EINVAL;
    }

    if (number_of_threads_long > max || number_of_threads_long > INT_MAX) {
        fputs ("the number of threads is too big\n", stderr);
        return EINVAL;
    }

    *number_of_threads = (int) number_of_threads_long;
    return SUCCESS;
}

Payload*
ThreadPayloadsCreate (int number_of_threads) {
    return (Payload *) malloc(sizeof (Payload) * number_of_threads);
}

void
ThreadPayloadsDelete (void *payloads) {
    free (payloads);
}

void
ThreadPayloadsInit (Payload *payloads, int number_of_threads, int number_of_iterations_per_chunk) {
    int i;
    for (i = 0; i < number_of_threads; ++i) {
        payloads[i].start = i;
        payloads[i].step = number_of_threads;
        payloads[i].chunk = number_of_iterations_per_chunk;
    }
}

void *
CalculatePI (void *arg) {
    Payload *payload = (Payload *) arg;
    int i;
    double pi_part = 0;

    int chunk = payload->chunk;
    int step = payload->step;
    int start = payload->start;
    int finish = chunk;

    while (!should_stop) {
        for (i = start; i < finish; i += step) {
            pi_part += 1.0 / (i * 4.0 + 1.0);
            pi_part -= 1.0 / (i * 4.0 + 3.0);
        }

        start += chunk;
        finish += chunk;

        if (finish < 0) {
            // index overflowed: stop
            break;
        }
    }

    payload->pi_part = pi_part;

    pthread_exit(arg);
}

int
StartParallelPiCalculation (pthread_t *threads, int number_of_threads, const Payload *payloads) {
    struct sigaction interrupt_sigaction;
    interrupt_sigaction.__sigaction_u.__sa_handler = interrupt_handler;
    interrupt_sigaction.sa_flags = SA_RESETHAND; // reset handler to default after first signal
    interrupt_sigaction.sa_mask = 0;
    int code;
    code = sigaction (SIGINT, &interrupt_sigaction, NULL);
    if (code != SUCCESS) {
        fprintf (stderr, "Couldn't set interrupt signal handler\n");
        return code;
    }

    int i;
    for (i = 0; i < number_of_threads; ++i) {
        code = pthread_create (threads + i, DEFAULT_ATTRS, CalculatePI, (void *) (payloads + i));
        if (code != SUCCESS) {
            fprintf(stderr, "Couldn't create thread #%d\n", i);
            return code;
        }
    }
    return SUCCESS;
}

int
FinishParallelPiCalculation (pthread_t *threads, int number_of_threads, double *pi_ptr) {
    void *status;
    Payload *current_payload_ptr;
    double pi = 0;
    int code;
    int i;
    for (i = 0; i < number_of_threads; ++i) {
        code = pthread_join (threads[i], &status);
        if (code != SUCCESS) {
            fprintf(stderr, "Couldn't join thread #%d\n", i);
            return code;
        }

        current_payload_ptr = (Payload *) status;
        pi += current_payload_ptr->pi_part;
    }

    *pi_ptr = pi * 4;
    return SUCCESS;
}
