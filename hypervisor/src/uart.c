#include "uart.h"

#define UART0_BASE 0x09000000UL  // correct for QEMU virt machine

static inline void mmio_write(unsigned long reg, unsigned int val) {
    *(volatile unsigned int *)reg = val;
}

static inline unsigned int mmio_read(unsigned long reg) {
    return *(volatile unsigned int *)reg;
}

void uart_init(void) {
    // QEMU’s UART doesn’t need init
}

void uart_putc(char c) {
    while (mmio_read(UART0_BASE + 0x18) & (1 << 5)); // TXFF
    mmio_write(UART0_BASE, c);
}

void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}
