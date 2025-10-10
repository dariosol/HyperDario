#include "uart.h"
//mrs %0, CurrentEL moves the value of the system register CurrentEL 
//into a general-purpose register (and then into the C variable el).
static inline unsigned int get_current_el(void) {
    unsigned long el;
    asm volatile ("mrs %0, CurrentEL" : "=r"(el));
    return (el >> 2) & 3;
}

void main(void) {
    uart_init();
    uart_puts("HyperDario starting...\n");

    unsigned int el = get_current_el();
    uart_puts("Current EL: ");
    uart_putc('0' + el);
    uart_puts("\n");

    while (1) {
        uart_puts("Running...\n");
        for (volatile int i = 0; i < 10000000; i++);
    }
}
