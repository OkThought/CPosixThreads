#include "dinner.h"
#include "bubble_sort.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define DEFAULT_ATTR NULL

typedef pthread_mutex_t      Fork;
typedef int                  Plate;
typedef pthread_mutex_t      EatAllowanceLock;
typedef pthread_cond_t       EatAllowance;

struct Table {
    Fork                    *forks;
    Plate                   *plates;
    EatAllowance            *eat_allowances;
    EatAllowanceLock        *eat_allowance_locks;
    unsigned                 number_of_seats;
};

struct DinnerInvitation {
    int                      seat_id;
    Table                   *table;
};

/*
 * private function declarations
 */

static void                 *philosopher_start (void *arg);
static void                  philosopher_eat_dinner (Table *table, int seat_id, Plate *plate);
static Fork                 *get_left_fork (Table *table, int seat_id);
static Fork                 *get_right_fork (Table *table, int seat_id);
static void                  eat_spaghetti (Plate *spaghetti_plate, useconds_t eat_microseconds);
static int                   take_both_forks_at_once (Fork *left_fork, Fork *right_fork);
static int                   take_fork (Fork *fork);
static int                   try_to_take_fork (Fork *fork);
static int                   put_fork (Fork *fork);
static int                   plate_is_empty (const Plate *plate);
static int                   serve_the_table (Table *table);
static void                  clean_the_table (Table *table);
static int                   random_int(int min, int max);
static void                  fill_the_plates_randomly (Table *table, int min_food, int max_food);
static int                   wait_for_eat_allowance (EatAllowance *eat_allowance, EatAllowanceLock *eat_allowance_lock);
static int                   allow_eat (EatAllowance *eat_allowances);
static int                   int_ptr_comparator_descending (void **a, void **b);
static void                  int_ptr_swap (void **a, void **b);
static void                  control_eating_priority (Table *table);

static const useconds_t      EAT_SPAGHETTI_PEACE_MICROSECONDS = 0;
static const useconds_t      THINKING_TIME_MICROSECONDS = 0;
static const unsigned        AMOUNT_OF_SPAGHETTI_TO_EAT_AT_ONCE = 1;
static void                **IGNORE_STATUS = NULL;
static void                 *NO_STATUS = NULL;

static pthread_mutex_t       printing_mutex = PTHREAD_MUTEX_INITIALIZER;




/*
 * private function definitions
 */

void *
philosopher_start (void *arg) {
    DinnerInvitation *invitation = (DinnerInvitation *) arg;
    int seat_id = invitation->seat_id;
//    printf ("philosopher %d: start\n", seat_id);
    Table *table = invitation->table;
    Plate *plate = &table->plates[seat_id];
    philosopher_eat_dinner (table, seat_id, plate);
    pthread_exit (NO_STATUS);
}

void
philosopher_eat_dinner (Table *table, int seat_id, Plate *plate) {
    Fork *left_fork = get_left_fork (table, seat_id);
    Fork *right_fork = get_right_fork (table, seat_id);
    EatAllowance *eat_allowance = &table->eat_allowances[seat_id];
    EatAllowanceLock *eat_allowance_lock = &table->eat_allowance_locks[seat_id];
    while (!plate_is_empty (plate)) {
//        printf ("philosopher %d: wait\n", seat_id);

        int code;
        code = pthread_mutex_lock (eat_allowance_lock);
        assert (code == 0);

        code = wait_for_eat_allowance (eat_allowance, eat_allowance_lock);
        assert (code == 0);

//        printf ("philosopher %d: allowed to eat\n", seat_id);

//        printf ("philosopher %d: take forks %d, %d\n", seat_id,
//                (int) (left_fork - table->forks),
//                (int) (right_fork - table->forks));

        code = take_both_forks_at_once (left_fork, right_fork);
        assert (code == 0);

//        printf ("philosopher %d: start eating, spaghetti left: %d\n", seat_id, *plate);

        int i;
        for (i = 0; i < AMOUNT_OF_SPAGHETTI_TO_EAT_AT_ONCE; ++i) {
            if (plate_is_empty (plate)) break;
            eat_spaghetti (plate, EAT_SPAGHETTI_PEACE_MICROSECONDS);
        }

//        printf ("philosopher %d: stop eating, spaghetti left: %d\n", seat_id, *plate);
//        printf ("%d, %d\n", seat_id, *plate);



//        printf ("philosopher %d: put forks %d, %d\n", seat_id,
//                (int) (left_fork - table->forks),
//                (int) (right_fork - table->forks));

        code = pthread_mutex_unlock (eat_allowance_lock);
        assert (code == 0);

        code = put_fork (left_fork);
        assert (code == 0);

        code = put_fork (right_fork);
        assert (code == 0);

        pthread_mutex_lock (&printing_mutex);
        for(i = 0; i < table->number_of_seats; ++i) {
            printf ("%d\t", table->plates[i]);
        }
        printf ("\n");
        pthread_mutex_unlock (&printing_mutex);
//        printf ("philosopher %d: think\n", seat_id);
        usleep (THINKING_TIME_MICROSECONDS);
    }
}

Fork *
get_left_fork (Table *table, int seat_id) {
    return &table->forks[seat_id];
}

Fork *
get_right_fork (Table *table, int seat_id) {
    return &table->forks[(seat_id + 1) % table->number_of_seats];
}

int
take_fork (Fork *fork) {
    return pthread_mutex_lock (fork);
}

int
try_to_take_fork (Fork *fork) {
    return pthread_mutex_trylock (fork);
}

int
put_fork (Fork *fork) {
    return pthread_mutex_unlock (fork);
}

int
take_both_forks_at_once (Fork *left_fork, Fork *right_fork) {
    int code = take_fork (left_fork);
    if (code != 0) {
        return code;
    }
#define FORK_TAKEN 0
    while (try_to_take_fork (right_fork) != FORK_TAKEN) {
        code = put_fork (left_fork);
        if (code != 0) {
            return code;
        }
        code = take_fork (left_fork);
        if (code != 0) {
            return code;
        }
    }
    return 0;
}

int
plate_is_empty (const Plate *plate) {
    return *plate == 0;
}

void
eat_spaghetti (Plate *spaghetti_plate, useconds_t eat_microseconds) {
    (void) usleep(eat_microseconds);
    *spaghetti_plate -= 1;
}

int
serve_the_table (Table *table) {
    int i;
    int code;
    for (i = 0; i < table->number_of_seats; ++i) {
        code = pthread_mutex_init (&table->forks[i], DEFAULT_ATTR);
        if (code != SUCCESS) {
            return code;
        }
        code = pthread_mutex_init (&table->eat_allowance_locks[i], DEFAULT_ATTR);
        if (code != SUCCESS) {
            return code;
        }
        code = pthread_cond_init (&table->eat_allowances[i], DEFAULT_ATTR);
        if (code != SUCCESS) {
            return code;
        }
    }

    return SUCCESS;
}

void
clean_the_table (Table *table) {
    int i;
    int code;
    for (i = 0; i < table->number_of_seats; ++i) {
        code = pthread_mutex_destroy (&table->forks[i]);
        assert (code == SUCCESS);
        code = pthread_mutex_destroy (&table->eat_allowance_locks[i]);
        assert (code == SUCCESS);
        code = pthread_cond_destroy (&table->eat_allowances[i]);
        assert (code == SUCCESS);
    }
}

void
fill_the_plates_randomly (Table *table, int min_food, int max_food) {
    time_t tm;
    time (&tm);
    srand ((unsigned) tm);
    int i;
    for (i = 0; i < table->number_of_seats; ++i) {
        table->plates[i] = random_int (min_food, max_food);
    }
}

int
random_int(int min, int max) {
    return min + rand () % (max - min);
}

int
wait_for_eat_allowance (EatAllowance *eat_allowance, EatAllowanceLock *eat_allowance_lock) {
    return pthread_cond_wait (eat_allowance, eat_allowance_lock);
}

void
control_eating_priority (Table *table) {
    unsigned number_of_philosophers = table->number_of_seats;
    int *plates = table->plates;
    int *plate_pointers[number_of_philosophers];
    int i;
    for (i = 0; i < number_of_philosophers; ++i) {
        plate_pointers[i] = &plates[i];
    }

    while (1) {
        bubble_sort ((void **) plate_pointers, number_of_philosophers, int_ptr_comparator_descending, int_ptr_swap);
        if (*plate_pointers[0] <= 0) {
            break;
        }

        int first_biggest_plate_id = (int) (plate_pointers[0] - plates);
        int second_biggest_plate_id = (int) (plate_pointers[1] - plates);

        pthread_mutex_lock (&table->eat_allowance_locks[first_biggest_plate_id]);
        pthread_mutex_unlock (&table->eat_allowance_locks[first_biggest_plate_id]);
        allow_eat (&table->eat_allowances[first_biggest_plate_id]);

        pthread_mutex_lock (&table->eat_allowance_locks[second_biggest_plate_id]);
        allow_eat (&table->eat_allowances[second_biggest_plate_id]);
        pthread_mutex_unlock (&table->eat_allowance_locks[second_biggest_plate_id]);
    }

}

int
allow_eat (EatAllowance *eat_allowances) {
    return pthread_cond_signal (eat_allowances);
}

void
int_ptr_swap (void **a, void **b) {
    int **i = (int **) a;
    int **j = (int **) b;
    int *tmp = *i;
    *i = *j;
    *j = tmp;
}

int
int_ptr_comparator_descending (void **a, void **b) {
    int i = ** (int **) a;
    int j = ** (int **) b;
    return j - i;
}

/*
 * public function definitions
 */

Table *
CreateTable (unsigned number_of_seats) {
    Table *table = (Table *) malloc (sizeof (Table));
    table->number_of_seats = number_of_seats;
    table->forks = (Fork *) malloc (sizeof (Fork) * number_of_seats);
    table->plates = (Plate *) malloc (sizeof (Plate) * number_of_seats);
    table->eat_allowances = (EatAllowance *) malloc (sizeof (EatAllowance) * number_of_seats);
    table->eat_allowance_locks = (EatAllowanceLock *) malloc (sizeof (EatAllowanceLock) * number_of_seats);

    if (table == NULL || table->forks == NULL || table->plates == NULL) {
        DeleteTable (table);
        fprintf (stderr, "Not enough memory to create table for %d persons\n", number_of_seats);
        return NULL;
    }

    return table;
}

void
DeleteTable (Table *table) {
    if (table != NULL) {
        free (table->forks);
        free (table->plates);
    }
    free (table);
}

DinnerInvitation *
CreateDinnerInvitations (unsigned invitations_number) {
    return (DinnerInvitation *) malloc (sizeof (DinnerInvitation) * invitations_number);
}

void
DeleteDinnerInvitations (DinnerInvitation *invitations) {
    free (invitations);
}

int
PrepareDinnerInvitations (Table *table, DinnerInvitation *invitations, unsigned invitations_number) {
    int i;
    for (i = 0; i < invitations_number; ++i) {
        invitations[i].table = table;
        invitations[i].seat_id = i;
    }
    return SUCCESS;
}

int
WaiterControlTable (Table *table, Philosopher *philosophers, DinnerInvitation *dinnerInvitations,
                    unsigned spaghettiPerPlateMin, unsigned spaghettiPerPlateMax) {
    int code;
    code = serve_the_table (table);
    if (code != SUCCESS) {
        fputs ("Couldn't serve_the_table\n", stderr);
        return code;
    }

    fill_the_plates_randomly (table, spaghettiPerPlateMin, spaghettiPerPlateMax);

    code = DinnerBegin (philosophers, dinnerInvitations, table->number_of_seats);
    if (code != SUCCESS) {
        fputs ("Couldn't SeatPhilosophersAtTheTable\n", stderr);
        return code;
    }

    control_eating_priority (table);

    DinnerEnd (philosophers, table->number_of_seats);

    clean_the_table (table);

    return SUCCESS;
}

int
DinnerBegin (Philosopher *philosophers, DinnerInvitation *invitations, unsigned philosophers_number) {
    int i;
    int code;
    for (i = 0; i < philosophers_number; ++i) {
        // EAGAIN    The system lacked the necessary resources to create another thread, or the system-imposed limit
        //            on the total number of threads in a process PTHREAD_THREADS_MAX would be exceeded.
        // EINVAL    The value specified by attr is invalid.
        // EPERM     The caller does not have appropriate permission to set the required scheduling parameters
        //            or scheduling policy.
        code = pthread_create (&philosophers[i], DEFAULT_ATTR, philosopher_start, (void *) &invitations[i]);
        assert (code != EINVAL && code != EPERM);
        if (code != SUCCESS) {
            fprintf (stderr, "Couldn't create thread for philosopher %d\n", i);
            return code;
        }
    }
    return SUCCESS;
}

void
DinnerEnd (Philosopher *philosophers, unsigned philosophers_number) {
    int i;
    int code;
    for (i = 0; i < philosophers_number; ++i) {
        code = pthread_join (philosophers[i], IGNORE_STATUS);
        assert (code == 0);
    }
}
