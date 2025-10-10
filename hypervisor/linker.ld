ENTRY(_start)

SECTIONS
{
    /* Hypervisor code */
    . = 0x40000000;
    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }

    /* Guest code */
    . = 0x41000000;
    .guest : { KEEP(*(.guest*)) }

    /* BSS */
    . = ALIGN(16);
    .bss : { *(.bss*) }

    /* EL2 hypervisor stack */
    .stack_hv : {
        _stack_top = . + 0x4000;
        . = _stack_top;
    }

    /* EL1 guest stack */
    . = 0x42000000;
    .stack_el1 : {
        _el1_stack_top = . + 0x4000;
        . = _el1_stack_top;
    }
}
