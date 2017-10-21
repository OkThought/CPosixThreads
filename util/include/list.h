#pragma once

#define SUCCESS 0

typedef struct list_s List;
typedef struct list_node_s ListNode;

List 						*ListCreate (void (*) (void *));
void 						 ListDelete (List *);
int 						 ListInsertAt (List *, void *, int);
int 						 ListInsertLast (List *, void *);
int 						 ListInsertFirst (List *, void *);
ListNode 			    	*ListGetHead (List *);
ListNode 			    	*ListGetTail (List *);
int 						 ListGetSize (List *);
int 						 ListIsEmpty (List *);
ListNode 			    	*ListNodeGetNext (ListNode *);
void 						*ListNodeGetValue (ListNode *);
