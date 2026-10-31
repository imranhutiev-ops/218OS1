#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "bitonic.h"
#include "io.h"

static void line(void) {
    for (int i = 0; i < 77; i++) putchar('=');
    putchar('\n');
}

int main(int argc, char **argv) {
    SetConsoleOutputCP(65001);   /* чтобы русский текст выводился нормально */

    if (argc < 3) {
        printf("Использование: %s <число_потоков> <размер_массива> [pause]\n", argv[0]);
        printf("  pause - создать потоки и подождать Enter (чтобы посмотреть их число в диспетчере задач)\n");
        return 1;
    }
    int T = atoi(argv[1]);
    unsigned n0 = (unsigned)atoi(argv[2]);
    int pause = (argc > 3);
    if (T < 1 || n0 < 2) { printf("Ошибка: неверные аргументы\n"); return 1; }

    /* добиваем размер до степени двойки */
    unsigned N = 1;
    while (N < n0) N <<= 1;

    int *a_seq = (int *)malloc((size_t)N * sizeof(int));
    int *a_par = (int *)malloc((size_t)N * sizeof(int));
    if (!a_seq || !a_par) { printf("Ошибка: памяти не выделяется, надо мозги подкачать\n"); return 1; }

    /* одинаковые случайные данные для обеих версий (xorshift32) */
    unsigned x = 2463534242u;
    for (unsigned i = 0; i < N; i++) {
        int v = INT_MAX_V;
        if (i < n0) {
            x ^= x << 13; x ^= x >> 17; x ^= x << 5;
            v = (int)(x % 2000001u) - 1000000;
        }
        a_seq[i] = v;
        a_par[i] = v;
    }

    double t0 = now_ms();
    bitonic_sort_serial(a_seq, N);
    double t_seq = now_ms() - t0;

    double t_par = bitonic_sort_parallel(a_par, N, T, pause);

    /* проверка: массивы совпадают и отсортированы */
    int ok = 1;
    for (unsigned i = 0; i < n0; i++) {
        if (a_seq[i] != a_par[i]) ok = 0;
        if (i && a_par[i - 1] > a_par[i]) ok = 0;
    }

    double speedup = t_seq / t_par;
    double efficiency = speedup / T * 100.0;

    printf("Битоническая сортировка. Массив: %u элементов (дополнен до %u).\n", n0, N);
    line();
    printf("| Потоки | Время посл. (мс) | Время парал. (мс) | Ускорение | Эффективность |\n");
    line();
    printf("| %6d | %16.3f | %17.3f | %9.2f | %12.1f%% |\n", T, t_seq, t_par, speedup, efficiency);
    line();
    printf("Проверка результата: %s\n", ok ? "массив отсортирован и совпадает с последовательной версией (ОК)"
                                          : "ОШИБКА, результат неверный(((");

    free(a_seq);
    free(a_par);
    return ok ? 0 : 2;
}