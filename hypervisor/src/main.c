#include "uart.h"

void main(void) {
    uart_init();
    uart_puts("HyperDario starting at EL2...\n");

    while (1) {
        uart_puts("Running...\n");
        for (volatile int i = 0; i < 10000000; i++);
    }
}
