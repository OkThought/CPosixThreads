#include "list.h"
#include "err_check.h"

#include <stdio.h> // printf
#include <pthread.h> // pthread_*
#include <string.h> // strerror
#include <stdlib.h> // exit
#include <errno.h> // error definitions

#define THREAD_NUMBER 4
#define NUMBER_STRINGS_PER_THREAD 16
#define STRING_SIZE_MAX 32
#define SUCCESS 0
#define DEFAULT_ATTRS NULL
#define IGNORE_STATUS NULL

/*
 * Creates a null-terminated array of list pointers with specified ValueDestructor
 */
List **CreateArrayOfLists(int number_of_lists, void (*ValueDestructor) (void *)) {
    List **lists = (List **) malloc(sizeof(void *) * (number_of_lists + 1));

    if (lists == NULL) {
        return NULL;
    }

    int i;
    for (i = 0; i < number_of_lists; ++i) {
        lists[i] = ListCreate(ValueDestructor);
        if (lists[i] == NULL) {
            return NULL;
        }
    }

    lists[number_of_lists] = NULL; // terminating null

    return lists;
}

/*
 * Deletes a null-terminated array of list pointers with calling ListDelete on each pointer
 */
void DeleteArrayOfLists(void *p) {
    if (p == NULL) return;

    List **lists = (List **) p;
    int i;
    for (i = 0; lists[i] != NULL; ++i) {
        List *lp = lists[i];
        ListDelete(lp);
    }
    free(lists);
}

int InitStringListForThread(List *list_ptr, int thread_index, int number_of_strings) {
    int str_index;
    for (str_index = 0; str_index < number_of_strings; ++str_index) {
        char *str = (char *) malloc(sizeof(str) * STRING_SIZE_MAX);
        if (str == NULL) {
            fputs ("couldn't malloc str\n", stderr);
            return ENOMEM;
        }

        snprintf (str, STRING_SIZE_MAX, "thread #%d: string #%d", thread_index, str_index);

        int code = ListInsertLast(list_ptr, str);
        if (code != 0) {
            fputs("couldn't ListInsertLast\n", stderr);
            return code;
        }
    }
    return SUCCESS;
}

void *Run(void *arg) {
    List *list_ptr = (List *) arg;
    ListNode *node_ptr;
    for (node_ptr = ListGetHead(list_ptr); node_ptr != NULL; node_ptr = ListNodeGetNext(node_ptr)) {
        char *str = (char *) ListNodeGetValue(node_ptr);
        if (str != NULL) {
            puts(str);
        }
    }
    pthread_exit(NULL);
}

int main() {
    int exit_value = EXIT_SUCCESS;
    pthread_t threads[THREAD_NUMBER];

    List **lists = CreateArrayOfLists(THREAD_NUMBER, free);
    if (lists == NULL) {
        char *error_message = strerror(errno);
        fwrite(error_message, strlen(error_message), 1, stderr);
        exit(errno);
    }

    int ret_code;
    int i;
    for (i = 0; i < THREAD_NUMBER; ++i) {
        ret_code = InitStringListForThread(lists[i], i, NUMBER_STRINGS_PER_THREAD);
        ExitIfNonZeroWithFormattedMessage(ret_code, "Couldn't initialize list of strings for thread #%d", i);
    }

    for (i = 0; i < THREAD_NUMBER; ++i) {
        ret_code = pthread_create(threads+i, DEFAULT_ATTRS, Run, (void*) lists[i]);
        if (ret_code != 0) {
            fprintf(stderr, "Error on pthread_create thread#%d: %s\n", i, strerror(ret_code));
            exit_value = EXIT_FAILURE;
            break;
        }
    }

    int number_threads_created = i;

    for (i = 0; i < number_threads_created; ++i) {
        ret_code = pthread_join(threads[i], IGNORE_STATUS);
        if (ret_code != 0) {
            fprintf(stderr, "Error on pthread_join thread#%d: %s\n", i, strerror(ret_code));
            exit_value = EXIT_FAILURE;
        }
    }

    DeleteArrayOfLists((void *) lists);
    exit(exit_value);
}


