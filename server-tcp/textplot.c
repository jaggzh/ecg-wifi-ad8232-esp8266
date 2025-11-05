#include "textplot.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>

static int term_width = 80;
static int term_height = 24;

static void handle_winch(int sig) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        term_width = w.ws_col;
        term_height = w.ws_row;
    }
}

void setup_textplot() {
    handle_winch(0); // Initial terminal size
    signal(SIGWINCH, handle_winch); // Terminal resize handler
}

void textplot(uint32_t us, uint16_t val) {
    static uint16_t min_val = 0xFFFF, max_val = 0;
    static int col = 0;
    static int first = 1;

    if (first) {
        setup_textplot();
        first = 0;
    }

    // Expand range immediately if out of bounds
    if (val < min_val) min_val = val;
    if (val > max_val) max_val = val;

    // Moving average tracking
    static int sample_count = 0;
    static uint32_t min_avg = 0, max_avg = 0;

    sample_count++;
    if (sample_count % 50 == 0) {
        min_avg = (min_avg * 9 + min_val) / 10;
        max_avg = (max_avg * 9 + max_val) / 10;
        min_val = min_avg;
        max_val = max_avg;
    }

    int range = max_val - min_val;
    if (range < 1) range = 1;

    int plot_row = (val - min_val) * (term_height - 1) / range;

    // Scroll effect: advance horizontally
    CURSOR_LEFT(col);
    CURSOR_UP(term_height - 1);

    for (int row = 0; row < term_height; row++) {
        if (row == plot_row) {
            CURSOR_RIGHT(col);
            putchar(PLOT_CHAR);
            CURSOR_LEFT(col + 1);
        }
        putchar('\n');
    }

    col = (col + 1) % term_width;
}
