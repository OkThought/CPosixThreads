#include "list.h"

#include <stdlib.h>
#include <errno.h>

struct list_node_s {
    void *value;
    struct list_node_s *next;
};

struct list_s {
    ListNode *head;
    ListNode *tail;
    int size;
    void (*ValueDestructor) (void *);
};

static ListNode *
list_node_create (void *value, ListNode *next) {
    ListNode *node_ptr = (ListNode *) malloc (sizeof (ListNode));
    if (node_ptr != NULL) {
        node_ptr->value = value;
        node_ptr->next = next;
    }
    return node_ptr;
}

static void
list_node_delete (ListNode *node_ptr) {
    free(node_ptr);
}

static int
add_value_to_empty_list(List *list_ptr, void *value) {
    ListNode *new_node_ptr = list_node_create (value, NULL);
    if (new_node_ptr == NULL) return errno;
    list_ptr->head = new_node_ptr;
    list_ptr->tail = new_node_ptr;
    list_ptr->size = 1;
    return SUCCESS;
}

List *
ListCreate (void (*ValueDestructor) (void *)) {
    List *list_ptr = (List *) malloc (sizeof (List));
    if (list_ptr != NULL) {
        list_ptr->head = NULL;
        list_ptr->tail = NULL;
        list_ptr->ValueDestructor = ValueDestructor;
    }
    return list_ptr;
}

void
ListDelete (List *list_ptr) {
    if (list_ptr == NULL) return;

    ListNode *next_node_ptr;
    ListNode *curr_node_ptr = list_ptr->head;
    list_ptr->head = list_ptr->tail = NULL;

    if (list_ptr->ValueDestructor == NULL) {
        for (; curr_node_ptr != NULL; curr_node_ptr = next_node_ptr) {
            next_node_ptr = curr_node_ptr->next;
            // printf("deleting node %p\n", curr_node_ptr);
            list_node_delete (curr_node_ptr);
        }
    } else {
        for (; curr_node_ptr != NULL; curr_node_ptr = next_node_ptr) {
            next_node_ptr = curr_node_ptr->next;
            void *value = curr_node_ptr->value;
            list_ptr->ValueDestructor (value);
            list_node_delete (curr_node_ptr);
        }
    }

    free(list_ptr);
}

int
ListInsertAt (List *list_ptr, void *value, int index) {
    if (list_ptr == NULL) return EINVAL;
    if (index < 0 || index > list_ptr->size) return EINVAL;
    if (index == 0) return ListInsertFirst(list_ptr, value);
    if (index == list_ptr->size) return ListInsertLast (list_ptr, value);

    ListNode *node_ptr = list_ptr->head;
    int i;
    for (i = 0; i < index; ++i) {
        node_ptr = node_ptr->next;
    }

    ListNode *new_node_ptr = list_node_create (value, node_ptr->next);
    if (new_node_ptr == NULL) return errno;
    node_ptr->next = new_node_ptr;
    ++(list_ptr->size);

    return SUCCESS;
}

int
ListInsertFirst (List *list_ptr, void *value) {
    if (list_ptr == NULL) return EINVAL;
    ListNode *head = list_ptr->head;
    if (head == NULL) return add_value_to_empty_list (list_ptr, value);
    ListNode *new_node_ptr = list_node_create (value, head);
    if (new_node_ptr == NULL) return errno;
    list_ptr->head = new_node_ptr;
    ++(list_ptr->size);
    return SUCCESS;
}

int
ListInsertLast (List *list_ptr, void *value) {
    if (list_ptr == NULL) return EINVAL;

    ListNode *tail = list_ptr->tail;
    if (tail == NULL) return add_value_to_empty_list (list_ptr, value);

    ListNode *new_node_ptr = list_node_create (value, NULL);
    if (new_node_ptr == NULL) return errno;

    tail->next = new_node_ptr;
    list_ptr->tail = new_node_ptr;

    ++(list_ptr->size);
    return SUCCESS;
}

ListNode *
ListGetHead (List *list_ptr) {
    if (list_ptr == NULL) {
        errno = EINVAL;
        return NULL;
    }
    return list_ptr->head;
}

ListNode *
ListGetTail (List *list_ptr) {
    if (list_ptr == NULL) {
        errno = EINVAL;
        return NULL;
    }
    return list_ptr->tail;
}

int
ListGetSize (List *list_ptr) {
    if (list_ptr == NULL) {
        return EINVAL;
    }
    return list_ptr->size;
}

int
ListIsEmpty(List *list_ptr) {
    if (list_ptr == NULL) {
        errno = EINVAL;
        return EINVAL;
    }
    return list_ptr->head == NULL || list_ptr->tail == NULL;
}

ListNode *
ListNodeGetNext (ListNode *node_ptr){
    if (node_ptr == NULL) {
        errno = EINVAL;
        return NULL;
    }
    return node_ptr->next;
}

void *
ListNodeGetValue (ListNode *node_ptr) {
    if (node_ptr == NULL) {
        errno = EINVAL;
        return NULL;
    }
    return node_ptr->value;
}
