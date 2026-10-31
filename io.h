#ifndef IO_H
#define IO_H

double now_ms(void);     /* текущее время в мс (QueryPerformanceCounter) */
void wait_enter(void);   /* ждать нажатия Enter */

#endif