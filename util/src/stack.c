#include "stack.h"

#include <stdlib.h>
#include <errno.h>

typedef struct stack_node_s {
	struct stack_node_s *next;
	void *value;
} StackNode;

struct stack_s {
	StackNode *head;
	int size;
	void (*ValueDestructor) (void *);
};

// private
StackNode *
stack_node_create (void *value, StackNode *next) {
	StackNode *np = (StackNode *) malloc(sizeof(np));
	if (np != NULL) {
		np->value = value;
		np->next = next;
	}
	return np;
}

// private
void 
stack_node_delete (StackNode *np) {
	free(np);
}

Stack *
StackCreate (void (*ValueDestructor) (void*)) {
	Stack *stack_ptr = (Stack *) malloc (sizeof(stack_ptr));
	if (stack_ptr != NULL) {
		stack_ptr->head = NULL;
		stack_ptr->size = 0;
		stack_ptr->ValueDestructor = ValueDestructor;
	}
	return stack_ptr;
}

void 
StackDelete (Stack *stack_ptr) {
	StackNode *next_node_ptr;
	StackNode *curr_node_ptr;

	if (stack_ptr == NULL) return;

	if (stack_ptr->ValueDestructor == NULL) {
		for (curr_node_ptr = stack_ptr->head; 
				 curr_node_ptr != NULL; 
				 curr_node_ptr = next_node_ptr) {
			next_node_ptr = curr_node_ptr->next;
			stack_node_delete (curr_node_ptr);
		}
	} else {
		for (curr_node_ptr = stack_ptr->head; 
				 curr_node_ptr != NULL; 
				 curr_node_ptr = next_node_ptr) {
			next_node_ptr = curr_node_ptr->next;
			void *value = curr_node_ptr->value;
			stack_ptr->ValueDestructor (value);
			stack_node_delete (curr_node_ptr);
		}
	}
	free(stack_ptr);
}

int 
StackPush (Stack *stack_ptr, void *value) {
	if (stack_ptr == NULL) {
		errno = EINVAL;
		return EINVAL;
	}
	
	StackNode *head = stack_ptr->head;
	StackNode *head_new = stack_node_create (value, head);

	if (head_new == NULL) {
		return errno;
	}

	stack_ptr->head = head_new;
	++(stack_ptr->size);
	return SUCCESS;
}

void *
StackPop (Stack *stack_ptr) {
	if (stack_ptr == NULL) {
		errno = EINVAL;
		return NULL;
	}

	StackNode *head = stack_ptr->head;
	
	if (head == NULL) {
		errno = EINVAL;
		return NULL;
	}
	
	StackNode *head_new = head->next;
	void *value = head->value;
	stack_node_delete (head);
	stack_ptr->head = head_new;
	--(stack_ptr->size);
	return value;
}

void *
StackPeek (Stack *stack_ptr) {
	if (stack_ptr == NULL) {
		errno = EINVAL;
		return NULL;
	}

	StackNode *head = stack_ptr->head;

	if (head == NULL) {
		errno = EINVAL;
		return NULL;
	}

	return head->value;
}

int
StackGetSize(Stack *stack_ptr) {
	if (stack_ptr == NULL) return EINVAL;
	return stack_ptr->size;
}

int
StackIsEmpty (Stack *stack_ptr) {
	return stack_ptr == NULL || stack_ptr->head == NULL;
}
