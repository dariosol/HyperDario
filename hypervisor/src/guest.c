// src/guest.c
#include "uart.h"

void guest_main(void) {
    uart_puts("Hello from EL1 guest!\n");
    while(1) {
        uart_puts("Guest running...\n");
        for (volatile int i = 0; i < 50000000; i++);
    }
}
