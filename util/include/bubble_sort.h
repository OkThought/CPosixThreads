#ifndef UTIL_BUBBLE_SORT_H
#define UTIL_BUBBLE_SORT_H

void             bubble_sort (void **values, int nvalues, int (*comparator) (void **, void **),
                              void (*swap) (void **, void **));

#endif //UTIL_BUBBLE_SORT_H
