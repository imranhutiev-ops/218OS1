#ifndef BITONIC_H
#define BITONIC_H

#define INT_MAX_V 2147483647

/* Один этап (k, j) для индексов i из [lo, hi).
   Пара (i, i ^ j) обрабатывается тем, у кого i меньше,
   поэтому разные потоки на одном этапе не пересекаются. */
static inline void bitonic_stage(int *a, unsigned k, unsigned j,
                                 unsigned lo, unsigned hi) {
    for (unsigned i = lo; i < hi; i++) {
        unsigned l = i ^ j;
        if (l > i) {
            int up = ((i & k) == 0);   /* направление сортировки блока */
            if (up ? (a[i] > a[l]) : (a[i] < a[l])) {
                int t = a[i];
                a[i] = a[l];
                a[l] = t;
            }
        }
    }
}

/* N обязано быть степенью двойки */
void bitonic_sort_serial(int *a, unsigned N);

/* T потоков, включая вызывающий. pause=1: потоки создаются приостановленными
   и ждут Enter (чтобы посмотреть их число в диспетчере задач).
   Возвращает время сортировки в мс. */
double bitonic_sort_parallel(int *a, unsigned N, int T, int pause);

#endif