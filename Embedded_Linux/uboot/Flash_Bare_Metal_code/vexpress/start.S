/*
 * start.S - Entry point for bare-metal program
 * 
 * This file handles:
 *   1. Saving U-Boot's return address
 *   2. Calling main()
 *   3. Returning to U-Boot
 * 
 * U-Boot passes: r0 = argc, r1 = argv
 */

.global _start
.section .text.start

_start:
    /*
     * Save callee-saved registers and link register (LR)
     * LR contains the return address back to U-Boot
     * We use U-Boot's stack (no need to set our own)
     */
    push {r4-r11, lr}
    
    /*
     * Call main(argc, argv)
     * r0 = argc (already set by U-Boot)
     * r1 = argv (already set by U-Boot)
     */
    bl main
    
    /*
     * Restore registers and return to U-Boot
     * pop {pc} loads saved LR directly into PC
     * This returns to U-Boot
     */
    pop {r4-r11, pc}