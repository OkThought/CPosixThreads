#ifndef UTIL_HELPERS_H
#define UTIL_HELPERS_H

#include <pthread.h>

#define SUCCESS 0
#define DEFAULT_ATTR NULL

static const int MAX_NUMBER_OF_THREADS = 100;

typedef struct PiCalcTask PiCalcTask;

void         PrintUsage ();
int          ParseNumberOfThreads (const char *number_of_threads_string, int *number_of_threads, int max);
PiCalcTask*  PiCalcTasksCreate (int number_of_threads);
void         PiCalcTasksDelete (void *tasks);
void         PiCalcTasksInit (PiCalcTask *tasks, int number_of_threads, int number_of_iterations);
int          StartParallelPiCalculation (pthread_t *thread_ptr, int number_of_threads, const PiCalcTask *tasks);
int          FinishParallelPiCalculation (pthread_t *thread_ptr, int number_of_threads, double *pi_ptr);



#endif //UTIL_HELPERS_H
