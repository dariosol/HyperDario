ENTRY(_start)

SECTIONS {
    . = 0x40000000; /* Load address for virt */
    .text : {
        *(.text*)
    }
    .rodata : {
        *(.rodata*)
    }
    .data : {
        *(.data*)
    }
    .bss : {
        *(.bss*)
        . = ALIGN(16);
        _stack_top = . + 0x4000;
    }
}
