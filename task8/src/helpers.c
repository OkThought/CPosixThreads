#include "helpers.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

#define FALSE 0
#define TRUE 1

struct ThreadTask {
    int start;              // initial  value for i in the loop (including)
    int step;               // step with which to iterate through the loop
    int chunk;              // number of iterations per chunk
    double pi_part;         // partial sum
};

int should_stop = FALSE;

static void        *calculate_pi (void *);

#ifdef __APPLE__
void                interrupt_handler (int sig);
#else
void                interrupt_handler ();
#endif

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

ThreadTask*
ThreadTasksCreate (int number_of_threads) {
    return (ThreadTask *) malloc(sizeof (ThreadTask) * number_of_threads);
}

void
ThreadTasksDelete (void *tasks) {
    free (tasks);
}

void
ThreadTasksInit (ThreadTask *tasks, int number_of_threads, int number_of_iterations_per_chunk) {
    int i;
    for (i = 0; i < number_of_threads; ++i) {
        tasks[i].start = i;
        tasks[i].step = number_of_threads;
        tasks[i].chunk = number_of_iterations_per_chunk;
    }
}

int
StartParallelPiCalculation (pthread_t *threads, int number_of_threads, const ThreadTask *tasks) {
    struct sigaction interrupt_sigaction;
#ifdef __APPLE__
    interrupt_sigaction.__sigaction_u.__sa_handler = interrupt_handler;
#else
    interrupt_sigaction.sa_handler = interrupt_handler;
#endif
    interrupt_sigaction.sa_flags = SA_RESETHAND; // reset handler to default after first signal
    sigemptyset (&interrupt_sigaction.sa_mask); // empty set of blocked signals

    int code;
    code = sigaction (SIGINT, &interrupt_sigaction, NULL);
    if (code != SUCCESS) {
        fprintf (stderr, "Couldn't set interrupt signal handler\n");
        return code;
    }

    int i;
    for (i = 0; i < number_of_threads; ++i) {
        code = pthread_create (threads + i, DEFAULT_ATTR, calculate_pi, (void *) (tasks + i));
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
    ThreadTask *current_task_ptr;
    double pi = 0;
    int code;
    int i;
    for (i = 0; i < number_of_threads; ++i) {
        code = pthread_join (threads[i], &status);
        if (code != SUCCESS) {
            fprintf(stderr, "Couldn't join thread #%d\n", i);
            return code;
        }

        current_task_ptr = (ThreadTask *) status;
        pi += current_task_ptr->pi_part;
    }

    *pi_ptr = pi * 4;
    return SUCCESS;
}

void
#ifdef __APPLE__
interrupt_handler (int sig) {
#else
    interrupt_handler () {
#endif
    puts ("\rStopping");
    should_stop = TRUE;
}

void *
calculate_pi (void *arg) {
    ThreadTask *task = (ThreadTask *) arg;
    int i;
    double pi_part = 0;

    int chunk = task->chunk;
    int step = task->step;
    int start = task->start;
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

    task->pi_part = pi_part;

    pthread_exit(arg);
}
