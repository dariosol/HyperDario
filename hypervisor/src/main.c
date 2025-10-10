#include "uart.h"

extern void enter_el1(void *guest_entry);
extern void guest_main(void);


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
    uart_puts("Current EL before enter_el1: ");
    uart_putc('0' + el);
    uart_puts("\n");

    // Jump to EL1 guest
    enter_el1(guest_main);

    while(1);

    uart_puts("Back to EL2 (unexpected)\n");
}
