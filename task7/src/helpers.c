#include "helpers.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

struct ThreadTask {
    int start_index;        // initial  value for i in the loop (including)
    int finish_index;       // final    value for i in the loop (excluding)
    double pi_part;         // partial sum
};

static void        *calculate_pi (void *);

void
PrintUsage () {
    fputs ("Usage:\t<program_name> <number_of_threads>\n\n", stderr);
    fprintf (stderr, "\t<number_of_threads> - number of threads to run pi calculation on. Maximum: %d\n",
             MAX_NUMBER_OF_THREADS);
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
        PrintUsage ();
        return errno;
    };

    if (*first_invalid_char != '\0') {
        fprintf (stderr, "'%s' is not a number\n", number_of_threads_string);
        PrintUsage ();
        return EINVAL;
    }

    if (number_of_threads_long <= 0) {
        fputs ("number_of_threads is not positive\n", stderr);
        PrintUsage ();
        return EINVAL;
    }

    if (number_of_threads_long > max || number_of_threads_long > INT_MAX) {
        fputs ("the number of threads is too big\n", stderr);
        PrintUsage ();
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
ThreadTasksInit (ThreadTask *tasks, int number_of_threads, int number_of_iterations) {
    int iterations_per_thread = number_of_iterations / number_of_threads;
    int iterations_rest = number_of_iterations - number_of_threads * iterations_per_thread;
    int prev_begin = 0;

    int i;
    for (i = 0; i < number_of_threads; ++i) {
        tasks[i].start_index = prev_begin;
        prev_begin = tasks[i].finish_index = prev_begin + iterations_per_thread + (i < iterations_rest);
    }
}

int
StartParallelPiCalculation (pthread_t *threads, int number_of_threads, const ThreadTask *tasks) {
    int code;
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


void *
calculate_pi (void *arg) {
    ThreadTask *task = (ThreadTask *) arg;
    int i;
    double pi_part = 0;
    for (i = task->start_index; i < task->finish_index; i++) {
        pi_part += 1.0/(i*4.0 + 1.0);
        pi_part -= 1.0/(i*4.0 + 3.0);
    }

    task->pi_part = pi_part;

    pthread_exit(arg);
}
