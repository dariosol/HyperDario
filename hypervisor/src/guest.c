// src/guest.c
#include "uart.h"

static inline void do_hyp_hvc(void) {
    asm volatile("svc #0");
}

void guest_main(void) {
    uart_puts("Hello from EL1 guest!\n");
    uart_puts("Guest: about to HVC\n");
    do_hyp_hvc();
    uart_puts("Guest: HVC returned?\n");
    while(1) {
        uart_puts("Guest running...\n");
        for (volatile int i = 0; i < 50000000; i++);
    }
}
