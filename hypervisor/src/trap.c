#include "uart.h"
#include <stdint.h>

void el2_handle_exception_c(unsigned long reason) {
    // reason: 0 sync, 1 irq, 2 fiq, 3 serror (as passed from vector)
    unsigned long esr = 0, elr = 0, far = 0, cur_el = 0;
    asm volatile("mrs %0, ESR_EL2" : "=r"(esr));
    asm volatile("mrs %0, ELR_EL2" : "=r"(elr));
    asm volatile("mrs %0, FAR_EL2" : "=r"(far));
    asm volatile("mrs %0, CurrentEL" : "=r"(cur_el));
    cur_el = (cur_el >> 2) & 3;

    uart_puts("== EL2 trap ==\n");
    uart_puts("trap type: ");
    uart_putc('0' + (reason & 0xF));
    uart_puts("\n");

    uart_puts("CurrentEL: ");
    uart_putc('0' + (int)cur_el);
    uart_puts("\n");

    uart_puts("ESR_EL2: ");
    // crude hex print
    for (int i = (sizeof(esr)/4)-1; i >= 0; --i) {
        unsigned long shift = i*32;
        unsigned int part = (esr >> shift) & 0xFFFFFFFF;
        // print 8 hex digits
        const char *hex = "0123456789ABCDEF";
        for (int nib = 7; nib >= 0; --nib) {
            unsigned int v = (part >> (nib*4)) & 0xF;
            uart_putc(hex[v]);
        }
    }
    uart_puts("\n");

    // ELR_EL2
    uart_puts("ELR_EL2: 0x");
    // print 64-bit hex quickly (simple)
    for (int i = 7; i >= 0; --i) {
        unsigned long part = (elr >> (i*8)) & 0xFF;
        unsigned int hi = (part >> 4) & 0xF;
        unsigned int lo = part & 0xF;
        const char *hex = "0123456789ABCDEF";
        uart_putc(hex[hi]); uart_putc(hex[lo]);
    }
    uart_puts("\n");

    uart_puts("FAR_EL2: 0x");
    for (int i = 7; i >= 0; --i) {
        unsigned long part = (far >> (i*8)) & 0xFF;
        unsigned int hi = (part >> 4) & 0xF;
        unsigned int lo = part & 0xF;
        const char *hex = "0123456789ABCDEF";
        uart_putc(hex[hi]); uart_putc(hex[lo]);
    }
    uart_puts("\n");

    uart_puts("== end trap ==\n");

    // For now: just return (eret in vector will restore guest PC -> if synchronous
    // instruction caused trap and should be emulated, you should modify ELR_EL2)
}
