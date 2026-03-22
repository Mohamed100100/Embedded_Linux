/*
 * main.c - Simple bare-metal program for U-Boot go command
 * 
 * This program prints a message to UART and returns to U-Boot.
 * 
 * Target: QEMU vexpress-a9 (or adjust UART_BASE for your board)
 */

/* UART Base Address - CHANGE FOR YOUR BOARD! */
#define UART_BASE   0x10009000      /* QEMU vexpress-a9 */

/* Common UART addresses for other boards:
 * BeagleBone Black:  0x44E09000
 * Raspberry Pi:      0x3F201000
 * i.MX6:             0x02020000
 */

#define UART_DR     (*(volatile unsigned int *)(UART_BASE + 0x00))

/* Print single character to UART */
void uart_putc(char c)
{
    UART_DR = c;
}

/* Print string to UART */
void uart_puts(const char *str)
{
    while (*str) {
        uart_putc(*str);
        str++;
    }
}

/* Print number in hexadecimal */
void uart_put_hex(unsigned int num)
{
    char hex_chars[] = "0123456789ABCDEF";
    int i;
    
    uart_puts("0x");
    for (i = 28; i >= 0; i -= 4) {
        uart_putc(hex_chars[(num >> i) & 0xF]);
    }
}

/* Main function - called from start.S */
int main(int argc, char *argv[])
{
    int i;
    
    uart_puts("\n\n");
    uart_puts("╔═══════════════════════════════════════════╗\n");
    uart_puts("║   Hello from Bare-Metal Program!          ║\n");
    uart_puts("║   Running on QEMU vexpress-a9             ║\n");
    uart_puts("╚═══════════════════════════════════════════╝\n");
    uart_puts("\n");
    
    /* Print argument count */
    uart_puts("Number of arguments: ");
    uart_putc('0' + argc);
    uart_puts("\n\n");
    
    /* Print arguments passed from U-Boot */
    uart_puts("Arguments from U-Boot:\n");
    for (i = 0; i < argc && i < 10; i++) {
        uart_puts("  argv[");
        uart_putc('0' + i);
        uart_puts("] = ");
        uart_puts(argv[i]);
        uart_puts("\n");
    }
    
    uart_puts("\n");
    uart_puts("Returning to U-Boot...\n");
    uart_puts("\n");
    
    return 0;  /* Return value shown by U-Boot */
}