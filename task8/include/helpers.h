#ifndef UTIL_HELPERS_H
#define UTIL_HELPERS_H

#include <pthread.h>

#define SUCCESS 0
#define DEFAULT_ATTR NULL

typedef struct Payload Payload;

void         PrintUsage();
int          ParseNumberOfThreads (const char *number_of_threads_string, int *number_of_threads, int max);
void        *CalculatePI (void *);
Payload*     ThreadPayloadsCreate (int number_of_threads);
void         ThreadPayloadsDelete (void *payloads);
void         ThreadPayloadsInit (Payload *payloads, int number_of_threads, int number_of_iterations_per_chunk);
int          StartParallelPiCalculation(pthread_t *thread_ptr, int number_of_threads, const Payload *payloads);
int          FinishParallelPiCalculation (pthread_t *thread_ptr, int number_of_threads, double *pi_ptr);



#endif //UTIL_HELPERS_H
