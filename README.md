BARE-METAL HYPERVISOR FOR ARM




CPU: ARM Cortex-A53 (ARMv8-A, 64-bit)

GOAL:
- A minimal build environment (cross-compiler, linker script, startup code)
- A bare-metal kernel that sets up:
	- Exception levels
	- MMU, page tables (still to be done)
	- UART for I/O
- Basic hypervisor functionality (trap handling, guest VM launch)

Use QEMU setup to simulate the hardware


REQUIREMENTS:
sudo apt install gcc-aarch64-linux-gnu qemu-system-aarch64 make
sudo apt install gdb-multiarch

start.S defines the hypervisor L2 stack (the main function)
el2_to_el1.S allows to switch from EL2 (Hyp space) to EL1(kernel space)

in EL1 stack the guest.c program runs.

stuff for uart here and there. Baseaddress for uart are the one for generic arm cpu in qemu

===================================================
To make:
make
make run
====================================================

make run in qemu:
    qemu-system-aarch64 -machine virt,virtualization=on -cpu cortex-a53 -kernel hypervisor.elf -nographic -serial mon:stdio

====================================================


Before enter_el1 (CPU in EL2)
─────────────────────────────
EL2 (Hypervisor)
─────────────────────────────
SP_EL2        -> hypervisor stack (_stack_top)
PC            -> executing main()
CurrentEL     -> 2 (EL2)
HCR_EL2       -> old value
SPSR_EL2      -> undefined for now
ELR_EL2       -> undefined
Other regs    -> guest and hypervisor can share general purpose regs

Stack:       [hypervisor stack memory]

Transition Steps:
1. Load _el1_stack_top into SP_EL1
2. Set HCR_EL2 (EL1 AArch64)
3. Set SPSR_EL2 (EL1h, interrupts enabled)
4. Set ELR_EL2 = guest_main
5. DSB + ISB -> Barriers to guarantee the registers have been written before going on
6. eret


After eret (CPU in EL1)
─────────────────────────────
EL1 (Guest)
─────────────────────────────
SP_EL1        -> _el1_stack_top (guest stack)
SP_EL2        -> unchanged (hypervisor stack)
PC            -> guest_main
CurrentEL     -> 1 (EL1)
SPSR_EL1      -> reflects state from SPSR_EL2 (EL1h, IRQ/FIQ enabled)
Other regs    -> inherited from before eret, or as set by guest

Stack:       [guest stack memory]

Hypervisor (EL2) still exists in background. If EL1 traps an exception (e.g., SVC, IRQ), CPU switches back to EL2.





          ┌───────────────┐
          │   EL2 (HV)    │
          │───────────────│
          │ SP_EL2        │─┐
          │ PC             │ │  Executes hypervisor
          │ HCR_EL2        │ │
          │ ELR_EL2        │ │
          └───────────────┘ │
                            │
            eret            │  Switches to EL1
                            │
          ┌───────────────┐ │
          │   EL1 (Guest) │ │
          │───────────────│ │
          │ SP_EL1        │─┘  Uses guest stack
          │ PC             │  Points to guest_main
          │ SPSR_EL1       │  EL1h, IRQ/FIQ enabled
          └───────────────┘










