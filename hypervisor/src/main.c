#include "uart.h"

extern void enter_el1(void *guest_entry);
extern void set_vbar_el2(void);
extern void guest_main(void);


//mrs %0, CurrentEL moves the value of the system register CurrentEL 
//into a general-purpose register (and then into the C variable el).
static inline unsigned int get_current_el(void) {
    unsigned long el;
    asm volatile ("mrs %0, CurrentEL" : "=r"(el));
    return (el >> 2) & 3;
}


static void uart_hex_digit(unsigned int d) {
    const char *hex = "0123456789ABCDEF";
    uart_putc(hex[d & 0xF]);
}

void uart_hex64(unsigned long long v) {
    for (int i = 7; i >= 0; --i) {
        unsigned int byte = (v >> (i*8)) & 0xFF;
        uart_hex_digit(byte >> 4);
        uart_hex_digit(byte & 0xF);
    }
}

#include <stdint.h>

/* write HCR_EL2 (value in v). Must be called from EL2. */
static inline void write_hcr_el2(uint64_t v) {
    asm volatile("msr HCR_EL2, %0\n\t"
                 "isb\n\t"
                 : : "r"(v) : "memory");
}


static inline unsigned long long read_vbar_el2(void) {
    unsigned long long v;
    asm volatile("mrs %0, VBAR_EL2" : "=r"(v));
    return v;
}
static inline unsigned long long read_hcr_el2(void) {
    unsigned long long v;
    asm volatile("mrs %0, HCR_EL2" : "=r"(v));
    return v;
}


void main(void) {
    uart_init();
    uart_puts("HyperDario starting...\n");
    set_vbar_el2();
    uart_puts("VBAR_EL2 set\n");


    // Prepare HCR_EL2 value: RW=1 (bit31), IMO/FMO/AMO = bits 3..5 set (trap IRQ/FIQ/SError)
    const uint64_t HCR_VAL = (1ULL << 31) | (7ULL << 3); // 0x80000038

    // Write it from EL2 (this inline asm will run at EL2 because main is EL2)
    write_hcr_el2(HCR_VAL);

    // Read back and print
    uint64_t hcr = read_hcr_el2();
    uart_puts("HCR_EL2=0x");
    uart_hex64(hcr);
    uart_puts("\n");

    uint64_t vbar = read_vbar_el2();
    uart_puts("VBAR_EL2=0x");
    uart_hex64(vbar);
    uart_puts("\n");

   
    unsigned int el = get_current_el();
    uart_puts("Current EL before enter_el1: ");
    uart_putc('0' + el);
    uart_puts("\n");

    // Jump to EL1 guest
    enter_el1(guest_main);

    while(1);

    uart_puts("Back to EL2 (unexpected)\n");
}
