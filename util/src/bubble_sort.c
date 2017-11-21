#include "bubble_sort.h"

void
bubble_sort (void **values, int nvalues, int (*comparator) (void **, void **),
             void (*swap) (void **, void **)) {
    int i, j;
    for (i = 0; i < nvalues - 1; ++i) {
        for (j = 0; j < nvalues - i - 1; ++j) {
            if (comparator(values + j, values + j+1) > 0) {
                swap (values + j, values + j+1);
            }
        }
    }
}
