#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>

static const int                 SUCCESS = 0;

typedef pthread_t                Philosopher;

typedef struct Table             Table;
typedef struct DinnerInvitation  DinnerInvitation;


Table                           *CreateTable (unsigned number_of_seats);

void                             DeleteTable (Table *table);

DinnerInvitation                *CreateDinnerInvitations (unsigned invitations_number);

void                             DeleteDinnerInvitations (DinnerInvitation *invitations);

int                              PrepareDinnerInvitations (Table *table,
                                                           DinnerInvitation *invitations,
                                                           unsigned invitations_number);

int                              WaiterControlTable (Table *table,
                                                     Philosopher *philosophers,
                                                     DinnerInvitation *dinnerInvitations,
                                                     unsigned spaghettiPerPlateMin,
                                                     unsigned spaghettiPerPlateMax);

int                              DinnerBegin (Philosopher *philosophers,
                                              DinnerInvitation *invitations,
                                              unsigned philosophers_number);

void                             DinnerEnd (Philosopher *philosophers,
                                            unsigned philosophers_number);

#endif //MAIN_H
