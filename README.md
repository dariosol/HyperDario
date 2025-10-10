Bare-Metal Hypervisor for ARM

Target CPU: ARM Cortex-A53 (ARMv8-A, 64-bit)

Goal

This project implements a minimal bare-metal hypervisor environment with the following features:

Build environment: Cross-compiler, linker scripts, and startup code

Bare-metal kernel setup including:

Exception levels

MMU and page tables (work in progress)

UART for I/O

Basic hypervisor functionality:

Trap handling

Guest VM launch

The hypervisor can be tested using a QEMU setup to simulate the hardware.

Requirements

Install the following dependencies on a Linux system:

sudo apt install gcc-aarch64-linux-gnu qemu-system-aarch64 make
sudo apt install gdb-multiarch

Key Source Files
File	Purpose
start.S	Defines the hypervisor L2 stack (main function)
el2_to_el1.S	Switches from EL2 (Hypervisor) to EL1 (Guest kernel)
guest.c	Runs in EL1 stack (guest program)
UART code	Base addresses correspond to generic ARM CPU in QEMU
Build & Run
Build
make

Run in QEMU
make run


or directly:

qemu-system-aarch64 \
    -machine virt,virtualization=on \
    -cpu cortex-a53 \
    -kernel hypervisor.elf \
    -nographic \
    -serial mon:stdio

CPU State Transition
Before enter_el1 (CPU in EL2)
─────────────────────────────
EL2 (Hypervisor)
─────────────────────────────
SP_EL2    -> hypervisor stack (_stack_top)
PC        -> executing main()
CurrentEL -> 2 (EL2)
HCR_EL2   -> old value
SPSR_EL2  -> undefined
ELR_EL2   -> undefined
Other regs -> guest and hypervisor can share general-purpose registers
Stack:     [hypervisor stack memory]


Transition Steps to EL1:

Load _el1_stack_top into SP_EL1

Set HCR_EL2 (EL1 AArch64)

Set SPSR_EL2 (EL1h, interrupts enabled)

Set ELR_EL2 = guest_main

DSB + ISB → Ensure registers are updated

eret → Switch to EL1

After eret (CPU in EL1)
─────────────────────────────
EL1 (Guest)
─────────────────────────────
SP_EL1    -> _el1_stack_top (guest stack)
SP_EL2    -> unchanged (hypervisor stack)
PC        -> guest_main
CurrentEL -> 1 (EL1)
SPSR_EL1  -> EL1h, IRQ/FIQ enabled
Other regs -> inherited from before eret or as set by guest
Stack:     [guest stack memory]


Hypervisor (EL2) still exists in the background.

Any EL1 exception (SVC, IRQ) switches the CPU back to EL2.

CPU State Diagram
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