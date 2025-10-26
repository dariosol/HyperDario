# Bare-Metal Hypervisor for ARM

**Target CPU:** ARM Cortex-A53 (ARMv8-A, 64-bit)

---

## Goal

This project implements a minimal bare-metal hypervisor environment with the following features:

* **Build environment**: Cross-compiler, linker scripts, and startup code
* **Bare-metal kernel** setup including:

  * Exception levels
  * MMU and page tables (work in progress)
  * UART for I/O
* **Basic hypervisor functionality**:

  * Trap handling
  * Guest VM launch

The hypervisor can be tested using a QEMU setup to simulate the hardware.

---

## Requirements

Install the following dependencies on a Linux system:

```bash
sudo apt install gcc-aarch64-linux-gnu qemu-system-aarch64 make
sudo apt install gdb-multiarch
```

---

## Key Source Files

| File           | Purpose                                              |
| -------------- | ---------------------------------------------------- |
| `start.S`      | Defines the hypervisor L2 stack (main function)      |
| `el2_to_el1.S` | Switches from EL2 (Hypervisor) to EL1 (Guest kernel) |
| `guest.c`      | Runs in EL1 stack (guest program)                    |
| UART code      | Base addresses correspond to generic ARM CPU in QEMU |

---

## Build & Run

### Build

```bash
make
```

### Run in QEMU

```bash
make run
```

or directly:

```bash
qemu-system-aarch64 \
    -machine virt,virtualization=on \
    -cpu cortex-a53 \
    -kernel hypervisor.elf \
    -nographic \
    -serial mon:stdio
```

---

## CPU State Transition

### Before `enter_el1` (CPU in EL2)

```
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
```

**Transition Steps to EL1:**

1. Load `_el1_stack_top` into `SP_EL1`
2. Set `HCR_EL2` (EL1 AArch64)
3. Set `SPSR_EL2` (EL1h, interrupts enabled)
4. Set `ELR_EL2 = guest_main`
5. `DSB + ISB` → Ensure registers are updated
6. `eret` → Switch to EL1

---

### After `eret` (CPU in EL1)

```
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
```

* Hypervisor (EL2) still exists in the background.
* Any EL1 exception (SVC, IRQ) switches the CPU back to EL2.

---

### CPU State Diagram

```
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
```

## VECTOR TABLES FOR EXCEPTIONS:
When you set up VBAR_EL2 (Vector Base Address Register), you're pointing to the start of a vector table. This table is a fixed-layout structure of 16 entries, each 0x80 bytes (128 bytes) apart.
The Four Groups (4 entries each)
The table is divided into 4 groups based on where the exception came from:
```
Offset   | Group Description
---------|------------------------------------------
0x000    | Current EL with SP_EL0
0x080    |   (4 entries: sync, irq, fiq, serror)
0x100    |
0x180    |
---------|------------------------------------------
0x200    | Current EL with SP_ELx  
0x280    |   (4 entries: sync, irq, fiq, serror)
0x300    |
0x380    |
---------|------------------------------------------
0x400    | Lower EL using AArch64  ← YOU NEEDED THIS!
0x480    |   (4 entries: sync, irq, fiq, serror)
0x500    |
0x580    |
---------|------------------------------------------
0x600    | Lower EL using AArch32
0x680    |   (4 entries: sync, irq, fiq, serror)
0x700    |
0x780    |
```

Within each group, the 4 entries handle different exception types:

1. 0x00: Synchronous (like HVC, system calls, data aborts)
2. 0x80: IRQ (interrupt request)
3. 0x100: FIQ (fast interrupt)
4. 0x180: SError (system error, asynchronous abort)

So the complete picture: 4 groups × 4 exception types = 16 entries total, each 0x80 bytes apart, for a total table size of 0x800 (2KB).
That's why the .align 11 directive is used—it aligns to 2^11 = 2048 bytes, which is the required alignment for the vector table!


### What Each Group Means
Current EL with SP_EL0 (0x000-0x180): Exceptions taken while running at EL2, but using the EL0 stack pointer (rare, special case)
Current EL with SP_ELx (0x200-0x380): Exceptions taken while running at EL2, using the EL2 stack pointer. This is what you originally had.
Lower EL using AArch64 (0x400-0x580): Exceptions taken from a lower exception level (EL1 or EL0) running in 64-bit mode. This is where your HVC instruction traps to!
Lower EL using AArch32 (0x600-0x780): Exceptions from lower ELs running in 32-bit mode (not relevant for this architecture)
### The Exception Level Ladder
Think of it as a ladder:
```
EL3 (Secure Monitor)     ← smc traps here
 ↑
EL2 (Hypervisor)         ← hvc traps here from EL0/EL1
 ↑
EL1 (Kernel/Guest OS)    ← svc traps here from EL0
 ↑
EL0 (Userspace/Apps)     ← where svc is called from
```
SVC vs HVC - Different Target Exception Levels

svc #0 (Supervisor Call) - traps from EL0 -> EL1
hvc #0 (Hypervisor Call) - traps from EL0/EL1 -> EL2
smc #0 (Secure Monitor Call) - traps from any EL -> EL3