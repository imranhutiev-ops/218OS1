#include <windows.h>
#include <stdio.h>
#include "io.h"

double now_ms(void) {
    LARGE_INTEGER c, f;
    QueryPerformanceCounter(&c);
    QueryPerformanceFrequency(&f);
    return (double)c.QuadPart * 1000.0 / (double)f.QuadPart;
}

void wait_enter(void) {
    getchar();
}