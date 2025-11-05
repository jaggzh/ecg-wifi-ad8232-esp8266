#ifndef _TEXTPLOT_H
#define _TEXTPLOT_H

#include <stdint.h>

void setup_textplot();
void textplot(uint32_t us, uint16_t val);

// ANSI Cursor Movement Macros
#define CURSOR_LEFT(n)   printf("\033[%dD", (n))
#define CURSOR_RIGHT(n)  printf("\033[%dC", (n))
#define CURSOR_UP(n)     printf("\033[%dA", (n))
#define CURSOR_DOWN(n)   printf("\033[%dB", (n))

#define PLOT_CHAR '*'

#endif // _TEXTPLOT_H
