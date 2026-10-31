#include "bitonic.h"

void bitonic_sort_serial(int *a, unsigned N) {
    for (unsigned k = 2; k <= N; k <<= 1)
        for (unsigned j = k >> 1; j > 0; j >>= 1)
            bitonic_stage(a, k, j, 0, N);
}