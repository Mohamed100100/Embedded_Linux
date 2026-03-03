/*
 * linker.ld - Linker script for bare-metal Raspberry Pi 3B+
 * 
 * Load Address Explanation:
 * ========================
 * We use 0x80000 (512KB) as our load address because:
 * 
 * 1. This is the default kernel load address for AArch64 on Raspberry Pi
 * 2. U-Boot's default 'kernel_addr_r' is typically set to 0x80000
 * 3. The first 512KB (0x00000-0x7FFFF) is reserved for:
 *    - Exception vectors
 *    - ARM stub code (armstub8.bin)
 *    - Firmware data structures
 * 4. Loading at 0x80000 ensures we don't conflict with bootloader/firmware
 * 5. U-Boot can easily load our binary here with: load mmc 0:1 0x80000 blinky.img
 * 
 * Memory Layout:
 * 0x00000000 - 0x0007FFFF: Reserved (ARM stub, vectors, etc.)
 * 0x00080000 - onwards:     Our bare-metal code starts here
 */

/* Entry point - the _start symbol from startup.s */
ENTRY(_start)

/* Define the load address */
LOAD_ADDRESS = 0x80000;

SECTIONS
{
    /* 
     * Place code at load address
     * The '.' represents the current location counter
     */
    . = LOAD_ADDRESS;
    
    /*
     * .text section - executable code
     * .text.boot comes first to ensure _start is at exactly 0x80000
     */
    .text : {
        KEEP(*(.text.boot))     /* Startup code MUST be first */
        *(.text .text.*)        /* All other code */
    }
    
    /*
     * .rodata section - read-only data (const variables, strings)
     * Align to 8-byte boundary for 64-bit access efficiency
     */
    . = ALIGN(8);
    .rodata : {
        *(.rodata .rodata.*)
    }
    
    /*
     * .data section - initialized global/static variables
     * Align to 8-byte boundary
     */
    . = ALIGN(8);
    .data : {
        *(.data .data.*)
    }
    
    /*
     * .bss section - uninitialized global/static variables
     * These must be zeroed by startup code before calling main()
     * We define __bss_start and __bss_end for the startup code
     */
    . = ALIGN(16);
    __bss_start = .;
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    }
    . = ALIGN(16);
    __bss_end = .;
    
    /* 
     * End of our binary
     * Useful for calculating total size
     */
    _end = .;
    
    /*
     * Discard sections we don't need
     * This keeps the binary small
     */
    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}