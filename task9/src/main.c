#include "dinner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define PHILOSOPHERS_NUMBER 5

static const unsigned SPAGHETTI_PER_PLATE = 100;
static Philosopher philosophers[PHILOSOPHERS_NUMBER];

int
main () {
    int code;

    Table *table = CreateTable (PHILOSOPHERS_NUMBER);
    if (table == NULL) {
        fprintf (stderr, "Couldn't CreateTable: %s\n", strerror (errno));
        exit (EXIT_FAILURE);
    }

    DinnerInvitation *dinnerInvitations = CreateDinnerInvitations (PHILOSOPHERS_NUMBER);
    if (dinnerInvitations == NULL) {
        DeleteTable (table);
        fprintf (stderr, "Couldn't CreateDinnerInvitations: %s\n", strerror (errno));
        exit (EXIT_FAILURE);
    }

    code = PrepareDinnerInvitations (table, dinnerInvitations, PHILOSOPHERS_NUMBER);
    if (code != SUCCESS) {
        DeleteTable (table);
        DeleteDinnerInvitations (dinnerInvitations);
        fprintf (stderr, "Couldn't PrepareDinnerInvitations: %s\n", strerror (code));
        exit (EXIT_FAILURE);
    }

    code = WaiterControlTable (table, philosophers, dinnerInvitations, SPAGHETTI_PER_PLATE);
    if (code != SUCCESS) {
        DeleteTable (table);
        DeleteDinnerInvitations (dinnerInvitations);
        fprintf (stderr, "Waiter failed controlling the table: %s\n", strerror (code));
        exit (EXIT_FAILURE);
    }

    DeleteTable (table);
    DeleteDinnerInvitations (dinnerInvitations);

    return 0;
}
