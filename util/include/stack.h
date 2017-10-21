#pragma once

#define SUCCESS 0

typedef struct stack_s Stack;

Stack 				    	*StackCreate(void (*) (void *));
void 						 StackDelete(Stack *);
int 						 StackPush(Stack *, void *);
void 						*StackPop(Stack *);
void 						*StackPeek(Stack *);
int 						 StackIsEmpty(Stack *);
int 						 StackGetSize(Stack *);
