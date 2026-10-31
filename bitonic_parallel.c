#define _WIN32_WINNT 0x0602
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "bitonic.h"
#include "io.h"

static int *g_a;
static unsigned g_N;
static int g_T;
static SYNCHRONIZATION_BARRIER g_bar;

static DWORD WINAPI worker(LPVOID p) {
    unsigned id = (unsigned)(INT_PTR)p;

    unsigned lo = (unsigned)((unsigned long long)g_N * id / (unsigned)g_T);
    unsigned hi = (unsigned)((unsigned long long)g_N * (id + 1) / (unsigned)g_T);

    for (unsigned k = 2; k <= g_N; k <<= 1)
        for (unsigned j = k >> 1; j > 0; j >>= 1) {
            bitonic_stage(g_a, k, j, lo, hi);
            EnterSynchronizationBarrier(&g_bar,
                SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY);
        }
    return 0;
}

double bitonic_sort_parallel(int *a, unsigned N, int T, int pause) {
    HANDLE *th = (HANDLE *)malloc((size_t)T * sizeof(HANDLE));
    if (!th) { printf("Ошибка: памяти не выделяется, надо мозги подкачать\n"); exit(1); }

    g_a = a; g_N = N; g_T = T;
    InitializeSynchronizationBarrier(&g_bar, (LONG)T, -1);

    DWORD flags = STACK_SIZE_PARAM_IS_A_RESERVATION | (pause ? CREATE_SUSPENDED : 0);

    double t0 = now_ms();
    for (int i = 1; i < T; i++) {
        th[i] = CreateThread(NULL, 64 * 1024, worker, (LPVOID)(INT_PTR)i, flags, NULL);
        if (!th[i]) { printf("Ошибка: не удалось создать потолок\n"); exit(1); }
    }

    if (pause) {
        printf("Потоки созданы. Глянь там в диспечере и Enter нажми...\n");
        fflush(stdout);
        wait_enter();
        t0 = now_ms();
        for (int i = 1; i < T; i++) ResumeThread(th[i]);
    }

    worker((LPVOID)0); 

    for (int i = 1; i < T; i++) {
        WaitForSingleObject(th[i], INFINITE);
        CloseHandle(th[i]);
    }
    double t = now_ms() - t0;

    DeleteSynchronizationBarrier(&g_bar);
    free(th);
    return t;
}