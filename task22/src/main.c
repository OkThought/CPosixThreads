#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define SUCCESS 0
#define NO_STATUS NULL
#define NO_ARGUMENT NULL
#define DEFAULT_ATTR NULL
#define SEM_PRIVATE 0
#define SEM_INIT_VALUE 0
#define PRODUCER_COUNT 5
#define MODULE 0
#define DETAIL_A 1
#define DETAIL_B 2
#define DETAIL_C 3
#define A_DETAIL_CREATION_TIME 1
#define B_DETAIL_CREATION_TIME 2
#define C_DETAIL_CREATION_TIME 3

#define SEM_NUMBER 4
static sem_t sem[SEM_NUMBER];

typedef enum {
    RUNNING, STOPPED
} program_state_t;
program_state_t global_state = RUNNING;

void SetGlobalState (program_state_t state) {
    global_state = state;
}

void SignalHandler (int signal_number) {
    if (signal_number == SIGINT) {
        SetGlobalState (STOPPED);
        sem_post (&sem[DETAIL_A]);
        sem_post (&sem[DETAIL_B]);
        sem_post (&sem[DETAIL_C]);
        sem_post (&sem[MODULE]);
    }
}

int SetSignalHandler () {
    errno = 0;
    if (signal (SIGINT, SignalHandler) == SIG_ERR) {
        return errno;
    }
    return SUCCESS;
}

int InitSemaphores () {
    int i;
    for (i = 0; i < SEM_NUMBER; ++i) {
        int code = sem_init (&sem[i], SEM_PRIVATE, SEM_INIT_VALUE);
        if (code != SUCCESS) {
            return code;
        }
    }
    return SUCCESS;
}

void destroy_semaphores () {
    int i;
    for (i = 0; i < SEM_NUMBER; ++i) {
        (void) sem_destroy (&sem[i]);
    }
}

void *produce_simple_detail (const char *detail_name,
                             unsigned int producing_timeout, sem_t *detail) {
    int detail_id = 0;
    (void) sleep (producing_timeout);
    while (global_state == RUNNING) {
        sem_post (detail);
        (void) printf ("detail %s-%d produced\n", detail_name, detail_id);
        detail_id++;
        (void) sleep (producing_timeout);
    }
    pthread_exit (NO_STATUS);
}

void *produce_detail_a (void *ignored) {
    return produce_simple_detail ("A", A_DETAIL_CREATION_TIME, &sem[DETAIL_A]);
}

void *produce_detail_b (void *ignored) {
    return produce_simple_detail ("B", B_DETAIL_CREATION_TIME, &sem[DETAIL_B]);
}

void *produce_detail_c (void *ignored) {
    return produce_simple_detail ("C", C_DETAIL_CREATION_TIME, &sem[DETAIL_C]);
}

void *produce_module (void *ignored) {
    int detail_a_id = 0;
    int detail_b_id = 0;
    int module_id = 0;
    sem_wait (&sem[DETAIL_A]);
    sem_wait (&sem[DETAIL_B]);
    while (global_state == RUNNING) {
        sem_post (&sem[MODULE]);
        (void) printf ("module-%d produced from (A-%d, B-%d)\n",
                module_id, detail_a_id, detail_b_id);
        ++detail_a_id;
        ++detail_b_id;
        ++module_id;

        sem_wait (&sem[DETAIL_A]);
        sem_wait (&sem[DETAIL_B]);
    }
    pthread_exit (NO_STATUS);
}

void *produce_widget (void *arg) {
    int widget_id = 0;
    int module_id = 0;
    int detail_c_id = 0;
    sem_wait (&sem[DETAIL_C]);
    sem_wait (&sem[MODULE]);
    while (global_state == RUNNING) {
        (void) printf ("widget-%d produced from (C-%d, M-%d)\n",
                widget_id, detail_c_id, module_id);
        widget_id++;
        detail_c_id++;
        module_id++;
        sem_wait (&sem[DETAIL_C]);
        sem_wait (&sem[MODULE]);
    }
    pthread_exit (NO_STATUS);
}

int start_all_producers (pthread_t *producers,
                         void *(*tasks[]) (void *), int producer_count) {
    int i;
    for (i = 0; i < producer_count; ++i) {
        int code = pthread_create (producers + i, DEFAULT_ATTR, tasks[i], NO_ARGUMENT);
        if (code != SUCCESS) {
            return code;
        }
    }
    return SUCCESS;
}

int join_all_producers (pthread_t *producers, int producer_count) {
    int code = 0;
    int i;
    for (i = 0; i < producer_count; ++i) {
        code = pthread_join (producers[i], NO_STATUS);
        if (code != SUCCESS) {
            return code;
        }
    }
    return SUCCESS;
}

int main () {
    int code = SetSignalHandler ();
    if (code != SUCCESS) {
        fprintf (stderr, "SIGINT handler was not set: %s\n", strerror (code));
    }

    code = InitSemaphores ();
    if (code != SUCCESS) {
        fprintf (stderr, "Unable to initialize semaphore: %s\n", strerror (code));
        destroy_semaphores ();
        exit (EXIT_FAILURE);
    }

    void *(*tasks[PRODUCER_COUNT]) (void *arg) = {
            produce_detail_a, produce_detail_b, produce_detail_c,
            produce_module, produce_widget
    };

    pthread_t producers[PRODUCER_COUNT];
    code = start_all_producers (producers, tasks, PRODUCER_COUNT);
    if (code != SUCCESS) {
        (void) fprintf (stderr, "Unable to start producers: %s\n", strerror (code));
        destroy_semaphores ();
        exit (EXIT_FAILURE);
    }

    code = join_all_producers (producers, PRODUCER_COUNT);
    if (code != SUCCESS) {
        (void) fprintf (stderr, "Unable to join producers: %s\n", strerror (code));
        destroy_semaphores ();
        exit (EXIT_FAILURE);
    }

    destroy_semaphores ();
    exit (EXIT_SUCCESS);
}
