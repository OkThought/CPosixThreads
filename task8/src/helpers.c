#include "helpers.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

#define IGNORE_OLD_SIGACTION NULL

struct PiCalcTask {
    int start;              // initial  value for i in the loop (including)
    int step;               // step with which to iterate through the loop
    double pi_part;         // partial sum
};

enum CalculationState {
    RUNNING, STOPPED
};

static enum CalculationState global_calculation_state = STOPPED;
static int global_chunk_size;
static int global_chunk_number = 0;
static pthread_mutex_t global_chunk_number_lock = PTHREAD_MUTEX_INITIALIZER;

#ifndef __APPLE__
static pthread_barrier_t global_barrier;
#endif

#ifdef __APPLE__
static void         interrupt_handler (int sig);
#else
static void         interrupt_handler ();
#endif

static void        *calculate_pi (void *);
static double       finish_pi_calculation (int chunk_counter, int chunk_start, int chunk_finish, int step);
static void         set_global_chunk_number_if_greater (int chunk_number);
int                 get_global_chunk_number ();

void
PrintUsage () {
    fputs("Usage:\t<program_name> number_of_threads\n", stderr);
    fputs("\tnumber_of_threads - number of threads to run pi calculation on.\n", stderr);
    fprintf (stderr, "\tnumber_of_threads should be in range %d...%d.\n",
             MIN_NUMBER_OF_THREADS, MAX_NUMBER_OF_THREADS);
}

int
ParseNumberOfThreads (const char *number_of_threads_string, int *number_of_threads, int min, int max) {
    if (number_of_threads_string == NULL)
        return EINVAL;

    errno = 0;
    static const int DECIMAL_BASE = 10;
    char *first_invalid_char;
    long number_of_threads_long = strtol (number_of_threads_string, &first_invalid_char, DECIMAL_BASE);
    if (errno != 0 || first_invalid_char == number_of_threads_string) {
        fprintf (stderr, "Couldn't parse int '%s' with base %d\n", number_of_threads_string, DECIMAL_BASE);
        PrintUsage ();
        return errno;
    };

    if (*first_invalid_char != '\0') {
        fprintf (stderr, "'%s' is not a number\n", number_of_threads_string);
        PrintUsage ();
        return EINVAL;
    }

    if (number_of_threads_long < min) {
        PrintUsage ();
        return EINVAL;
    }

    if (number_of_threads_long > max || number_of_threads_long > INT_MAX) {
        PrintUsage ();
        return EINVAL;
    }

    *number_of_threads = (int) number_of_threads_long;
    return SUCCESS;
}

PiCalcTask*
PiCalcTasksCreate (int number_of_threads) {
    return (PiCalcTask *) malloc(sizeof (PiCalcTask) * number_of_threads);
}

void
PiCalcTasksDelete (void *tasks) {
    free (tasks);
}

void
PiCalcTasksInit (PiCalcTask *tasks, int number_of_threads) {
    int i;
    for (i = 0; i < number_of_threads; ++i) {
        tasks[i].start = i;
        tasks[i].step = number_of_threads;
    }
}

int
StartParallelPiCalculation (pthread_t *threads, int number_of_threads, const PiCalcTask *tasks,
                            int number_of_iterations_per_chunk) {
    struct sigaction interrupt_sigaction;
#ifdef __APPLE__
    interrupt_sigaction.__sigaction_u.__sa_handler = interrupt_handler;
#else
    interrupt_sigaction.sa_handler = interrupt_handler;
#endif
    interrupt_sigaction.sa_flags = SA_RESETHAND; // reset handler to default after first signal
    sigemptyset (&interrupt_sigaction.sa_mask); // empty set of blocked signals

    int code;
    code = sigaction (SIGINT, &interrupt_sigaction, IGNORE_OLD_SIGACTION);
    if (code != SUCCESS) {
        fprintf (stderr, "Couldn't set interrupt signal handler\n");
        return code;
    }

#ifndef __APPLE__
    // EAGAIN          The system lacks the necessary resources  to initialize another barrier.
    // EINVAL          The value specified by count is equal to 0.
    // ENOMEM          Insufficient memory exists to initialize the barrier.
    // EINVAL          The value specified by attr is invalid.
    code = pthread_barrier_init(&global_barrier, DEFAULT_ATTR, number_of_threads);
    if (code != SUCCESS) {
        fprintf (stderr, "Couldn't init global_barrier\n");
        return code;
    }
#endif

    global_calculation_state = RUNNING;
    global_chunk_size = number_of_iterations_per_chunk;

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
    PiCalcTask *current_task_ptr;
    double pi = 0;
    int code;
    int i;
    for (i = 0; i < number_of_threads; ++i) {
        code = pthread_join (threads[i], &status);
        if (code != SUCCESS) {
            fprintf(stderr, "Couldn't join thread #%d\n", i);
            return code;
        }

        current_task_ptr = (PiCalcTask *) status;
        pi += current_task_ptr->pi_part;
    }

#ifndef __APPLE__
    // EBUSY           The implementation has detected an attempt to destroy a barrier while it is in use
    //                  (for example, while being used in a pthread_barrier_wait() call) by another thread.
    // EINVAL          The value specified by barrier is invalid.
    (void) pthread_barrier_destroy(&global_barrier);
#endif

    *pi_ptr = pi * 4;
    return SUCCESS;
}

void
interrupt_handler (
#ifdef __APPLE__
        int sig
#endif
                  ) {
    puts ("\rStopping");
    global_calculation_state = STOPPED;
}

double
calculate_chunk (int chunk_start, int chunk_finish, int step) {
    double pi_part = 0;
    int i;
    for (i = chunk_start; i < chunk_finish; i += step) {
        pi_part += 1.0 / (i * 4.0 + 1.0);
        pi_part -= 1.0 / (i * 4.0 + 3.0);
    }
    return pi_part;
}

void *
calculate_pi (void *arg) {
    PiCalcTask *task = (PiCalcTask *) arg;
    double pi_part = 0;
    int step = task->step;
    int chunk_start = task->start;
    int chunk_finish = global_chunk_size;
    int chunk_counter = 0;

    while (global_calculation_state == RUNNING) {
        pi_part += calculate_chunk (chunk_start, chunk_finish, step);

        ++chunk_counter;

        chunk_start += global_chunk_size;
        chunk_finish += global_chunk_size;

        if (chunk_finish < 0) {
            // iteration overflowed
            task->pi_part = pi_part;
            pthread_exit (arg);
        }
    }

    pi_part += finish_pi_calculation (chunk_counter, chunk_start, chunk_finish, step);

    task->pi_part = pi_part;
    pthread_exit (arg);
}

double
finish_pi_calculation (int chunk_counter, int chunk_start, int chunk_finish, int step) {
    double pi_part = 0;
    set_global_chunk_number_if_greater (chunk_counter);

#ifndef __APPLE__
    // EINVAL          The value specified by barrier does not refer to an initialized barrier object.
    (void) pthread_barrier_wait (&global_barrier);
#endif

    int chunk_number = get_global_chunk_number ();

    while (chunk_counter < chunk_number) {
        pi_part += calculate_chunk (chunk_start, chunk_finish, step);
        ++chunk_counter;
        chunk_start += global_chunk_size;
        chunk_finish += global_chunk_size;
    }
    return pi_part;
}

void
set_global_chunk_number_if_greater (int chunk_number) {
    pthread_mutex_lock (&global_chunk_number_lock);
    if (chunk_number > global_chunk_number) {
        global_chunk_number = chunk_number;
    }
    pthread_mutex_unlock (&global_chunk_number_lock);
}

int
get_global_chunk_number () {
    int chunk_number;
    pthread_mutex_lock (&global_chunk_number_lock);
    chunk_number = global_chunk_number;
    pthread_mutex_unlock (&global_chunk_number_lock);
    return chunk_number;
}
