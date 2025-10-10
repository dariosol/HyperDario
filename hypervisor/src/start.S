.section .text
.globl _start
_start:
    // Set up stack pointer for EL2
    ldr x0, =_stack_top
    mov sp, x0

    // Jump to C main
    bl main

1:  wfe
    b 1b
