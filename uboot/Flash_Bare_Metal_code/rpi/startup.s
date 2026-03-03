/*
 * startup.s - AArch64 bare-metal entry point for Raspberry Pi 3B+
 * 
 * This file is necessary because:
 * 1. We need to set up the stack pointer before any C code can run
 * 2. We need to ensure only one CPU core runs (others should halt)
 * 3. We need to zero the BSS section for proper C initialization
 * 4. We need a controlled entry point at a known address
 * 
 * C code cannot do this because C requires a stack to be already set up
 * for function calls, local variables, and parameter passing.
 */

.section .text.boot
.global _start

_start:
    /* 
     * Check processor ID - only let core 0 proceed
     * Other cores (1,2,3) will be put to sleep
     */
    mrs     x0, mpidr_el1       // Read Multiprocessor Affinity Register
    and     x0, x0, #0xFF       // Extract CPU ID (bits 0-7)
    cbz     x0, core0_boot      // If core 0, continue to boot
    
    /* Secondary cores: infinite wait loop */
secondary_hang:
    wfe                         // Wait For Event (low power)
    b       secondary_hang

core0_boot:
    /* 
     * Set up the stack pointer
     * Stack grows downward, so we set SP to top of our stack area
     * Using address 0x80000 (512KB) as stack top
     * Our code loads at 0x80000, stack is below it
     */
    ldr     x0, =_start
    mov     sp, x0              // Stack grows down from _start (0x80000)

    /*
     * Zero out the BSS section
     * BSS contains uninitialized global/static variables
     * C standard requires these to be zero-initialized
     */
    ldr     x0, =__bss_start    // Start of BSS
    ldr     x1, =__bss_end      // End of BSS
    
bss_clear_loop:
    cmp     x0, x1              // Compare current position with end
    b.ge    bss_clear_done      // If done, exit loop
    str     xzr, [x0], #8       // Store zero, increment pointer by 8 bytes
    b       bss_clear_loop

bss_clear_done:
    /*
     * Branch to C main function
     * If main() ever returns, we fall through to hang loop
     */
    bl      main

    /*
     * Hang loop - program should never reach here
     * If main() returns, we just loop forever
     */
hang:
    wfe                         // Wait For Event (low power state)
    b       hang

.size _start, . - _start