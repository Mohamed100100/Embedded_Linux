/*
 * linker.ld - Linker script for bare-metal program
 * 
 * Load address: 0x60100000
 * This must match where you load the binary in U-Boot!
 * 
 * Usage in U-Boot:
 *   load mmc 0:1 0x60100000 myprogram.bin
 *   go 0x60100000
 */

ENTRY(_start)

SECTIONS
{
    /* Starting address - must match U-Boot load address */
    . = 0x60100000;
    
    /* Code section */
    .text : {
        *(.text.start)    /* Entry point first */
        *(.text*)         /* All other code */
    }
    
    /* Read-only data */
    .rodata : {
        *(.rodata*)
    }
    
    /* Initialized data */
    .data : {
        *(.data*)
    }
    
    /* Uninitialized data */
    .bss : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }
}