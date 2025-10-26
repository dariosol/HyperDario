// enter_el1.S - Minimal EL2 to EL1 transition
.global enter_el1
.type enter_el1, %function

enter_el1:
    // --- Load EL1 stack pointer ---
    ldr x1, =_el1_stack_top
    msr SP_EL1, x1

    // --- Configure HCR_EL2 ---
    // RW=1 (EL1 executes AArch64), IMO/FMO/AMO=1 (trap interrupts/exceptions)
    mov x2, #(7 << 3)
    movk x2, #0x8000, lsl #16   // set bit 31 (RW=1)
    msr  HCR_EL2, x2

    // --- Configure SPSR_EL2 ---
    // EL1h (mode bits M[3:0] = 0b0101)
    // Interrupts unmasked (I=0, F=0, D=0)
    mov x3, #0x5
    msr SPSR_EL2, x3

    // --- Set ELR_EL2 to guest entry ---
    msr ELR_EL2, x0

    // --- Data and instruction barriers ---
    dsb ish
    isb

    // --- Return to EL1 ---
    eret

.size enter_el1, .-enter_el1
