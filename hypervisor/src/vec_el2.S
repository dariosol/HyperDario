/* vec_el2.S - EL2 vector table for AArch64 */

.section .text
.align 11
.global el2_vector_table
.global set_vbar_el2

// ====================================================
// Vector table layout (each entry is 0x80 bytes apart)
// ====================================================
el2_vector_table:

// ====================================================
// Current EL with SP_EL0 (0x000 - 0x180)
// ====================================================
.align 7
curr_el_sp0_sync:
    b .
.align 7
curr_el_sp0_irq:
    b .
.align 7
curr_el_sp0_fiq:
    b .
.align 7
curr_el_sp0_serror:
    b .

// ====================================================
// Current EL with SP_ELx (0x200 - 0x380)
// ====================================================
.align 7
el2_sync_entry:
    stp x0, x1, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    mov x0, #0
    bl el2_handle_exception_c
    ldp x6, x7, [sp], #16
    ldp x4, x5, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x0, x1, [sp], #16
    eret

.align 7
el2_irq_entry:
    stp x0, x1, [sp, #-16]!
    mov x0, #1
    bl el2_handle_exception_c
    ldp x0, x1, [sp], #16
    eret

.align 7
el2_fiq_entry:
    stp x0, x1, [sp, #-16]!
    mov x0, #2
    bl el2_handle_exception_c
    ldp x0, x1, [sp], #16
    eret

.align 7
el2_serror_entry:
    stp x0, x1, [sp, #-16]!
    mov x0, #3
    bl el2_handle_exception_c
    ldp x0, x1, [sp], #16
    eret

// ====================================================
// Lower EL using AArch64 (0x400 - 0x580)
// ====================================================
.align 7
lower_el_sync:
    stp x0, x1, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    mov x0, #0
    bl el2_handle_exception_c
    ldp x6, x7, [sp], #16
    ldp x4, x5, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x0, x1, [sp], #16
    eret

.align 7
lower_el_irq:
    stp x0, x1, [sp, #-16]!
    mov x0, #1
    bl el2_handle_exception_c
    ldp x0, x1, [sp], #16
    eret

.align 7
lower_el_fiq:
    stp x0, x1, [sp, #-16]!
    mov x0, #2
    bl el2_handle_exception_c
    ldp x0, x1, [sp], #16
    eret

.align 7
lower_el_serror:
    stp x0, x1, [sp, #-16]!
    mov x0, #3
    bl el2_handle_exception_c
    ldp x0, x1, [sp], #16
    eret

// ====================================================
// Lower EL using AArch32 (0x600 - 0x780)
// ====================================================
.align 7
lower_el_aarch32_sync:
    b .
.align 7
lower_el_aarch32_irq:
    b .
.align 7
lower_el_aarch32_fiq:
    b .
.align 7
lower_el_aarch32_serror:
    b .

// ====================================================
// Helper function to install vector base
// ====================================================
set_vbar_el2:
    adr x0, el2_vector_table
    msr VBAR_EL2, x0
    dsb ish
    isb
    ret