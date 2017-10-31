#ifndef UTIL_HELPERS_H
#define UTIL_HELPERS_H

#include <pthread.h>

#define SUCCESS 0
#define DEFAULT_ATTR NULL

typedef struct {
    int start_index;        // initial  value for i in the loop (including)
    int finish_index;       // final    value for i in the loop (excluding)
    double pi_part;         // partial sum
} ThreadTask;

void         PrintUsage ();
int          ParseNumberOfThreads (const char *number_of_threads_string, int *number_of_threads, int max);
ThreadTask*  ThreadTasksCreate (int number_of_threads);
void         ThreadTasksDelete (void *tasks);
void         ThreadTasksInit (ThreadTask *tasks, int number_of_threads, int number_of_iterations);
int          StartParallelPiCalculation (pthread_t *thread_ptr, int number_of_threads, const ThreadTask *tasks);
int          FinishParallelPiCalculation (pthread_t *thread_ptr, int number_of_threads, double *pi_ptr);



#endif //UTIL_HELPERS_H
