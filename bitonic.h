#ifndef BITONIC_H
#define BITONIC_H

#define INT_MAX_V 2147483647


static inline void bitonic_stage(int *a, unsigned k, unsigned j,
                                 unsigned lo, unsigned hi) {
    for (unsigned i = lo; i < hi; i++) {
        unsigned l = i ^ j;
        if (l > i) {
            int up = ((i & k) == 0);   
            if (up ? (a[i] > a[l]) : (a[i] < a[l])) {
                int t = a[i];
                a[i] = a[l];
                a[l] = t;
            }
        }
    }
}

void bitonic_sort_serial(int *a, unsigned N);

double bitonic_sort_parallel(int *a, unsigned N, int T, int pause);

#endif
