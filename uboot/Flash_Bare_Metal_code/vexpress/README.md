# README.md

```markdown
# Bare-Metal Program for U-Boot

A simple bare-metal program that runs via U-Boot's `go` command and returns back to U-Boot.

---

## Overview

This program demonstrates:
- Writing bare-metal code for ARM
- Printing to UART without any OS
- Properly returning to U-Boot after execution

---

## Files

| File | Description |
|------|-------------|
| `main.c` | Main program logic, UART printing |
| `start.S` | Assembly entry point, handles return to U-Boot |
| `linker.ld` | Linker script, defines memory layout |
| `Makefile` | Build instructions |
| `README.md` | This file |

---

## Requirements

- ARM cross compiler: `arm-linux-gnueabi-gcc`
- U-Boot running on target or QEMU
- SD card (for real hardware) or SD card image (for QEMU)

### Install Compiler (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install gcc-arm-linux-gnueabi
```

---

## Building

```bash
# Build the program
make

# Clean build files
make clean

# Generate disassembly (for debugging)
make disasm

# Copy to SD card
make install
```

---

## Running

### Option 1: QEMU

```bash
# Start QEMU with U-Boot
qemu-system-arm -M vexpress-a9 -kernel u-boot -nographic -sd sd_card.img
```

In U-Boot:
```
=> load mmc 0:1 0x60100000 myprogram.bin
=> go 0x60100000
```

### Option 2: Real Hardware

1. Copy `myprogram.bin` to SD card boot partition
2. Boot the board into U-Boot
3. Run:
```
=> load mmc 0:1 0x60100000 myprogram.bin
=> go 0x60100000
```

---

## Expected Output

```
=> load mmc 0:1 0x60100000 myprogram.bin
465 bytes read in 5 ms (90.8 KiB/s)

=> go 0x60100000
## Starting application at 0x60100000 ...

╔═══════════════════════════════════════════╗
║   Hello from Bare-Metal Program!          ║
║   Running on QEMU vexpress-a9             ║
╚═══════════════════════════════════════════╝

Number of arguments: 2

Arguments from U-Boot:
  argv[0] = go
  argv[1] = 0x60100000

Returning to U-Boot...

## Application terminated, rc = 0x0
=>
```

---

## Customization

### Change UART Address

Edit `main.c` and change `UART_BASE` for your board:

```c
/* Common UART addresses: */
#define UART_BASE   0x10009000   /* QEMU vexpress-a9 */
// #define UART_BASE   0x44E09000   /* BeagleBone Black */
// #define UART_BASE   0x3F201000   /* Raspberry Pi */
// #define UART_BASE   0x02020000   /* i.MX6 */
```

### Change Load Address

If you want to load at a different address:

1. Edit `linker.ld`:
```ld
. = 0x80000000;   /* New address */
```

2. Use matching address in U-Boot:
```
=> load mmc 0:1 0x80000000 myprogram.bin
=> go 0x80000000
```

---

## How It Works

### Program Flow

```
U-Boot                              Your Program
   │                                     │
   │ ──── go 0x60100000 ───────────────>│
   │                                     │
   │                              start.S: _start
   │                                │
   │                                ├── Save return address (LR)
   │                                ├── Call main()
   │                                │
   │                              main.c: main()
   │                                │
   │                                ├── Print to UART
   │                                ├── return 0
   │                                │
   │                              start.S:
   │                                │
   │                                └── Restore LR, return
   │                                     │
   │ <──── return ──────────────────────│
   │
## Application terminated, rc = 0x0
=>
```

### Why We Save LR (Link Register)

```
Problem:
─────────
  U-Boot calls _start
    → LR = return address to U-Boot
  
  _start calls main (bl main)
    → LR = address after bl instruction (OVERWRITES!)
  
  bx lr
    → Returns to wrong address! (infinite loop)

Solution:
─────────
  U-Boot calls _start
    → LR = return address to U-Boot
  
  push {lr}
    → Save LR to stack
  
  _start calls main (bl main)
    → LR changes, but saved copy is safe!
  
  pop {pc}
    → Restore saved LR directly to PC
    → Returns to U-Boot correctly!
```

---

## Memory Layout

```
┌─────────────────────────────────────────┐
│ 0x60100000: .text (code)               │ ← Entry point
│             _start                      │
│             main                        │
│             uart_putc                   │
│             uart_puts                   │
├─────────────────────────────────────────┤
│ 0x601001xx: .rodata (strings)          │
│             "Hello from..."             │
├─────────────────────────────────────────┤
│ 0x601002xx: .data (initialized data)   │
├─────────────────────────────────────────┤
│ 0x601003xx: .bss (uninitialized data)  │
└─────────────────────────────────────────┘
```

---

## U-Boot Commands Reference

| Command | Returns to U-Boot? | Description |
|---------|-------------------|-------------|
| `go <addr>` | YES (if code returns) | Jump to address and execute |
| `bootz <addr>` | NO (never) | Boot Linux kernel |
| `bootm <addr>` | NO (never) | Boot legacy kernel image |
| `load mmc 0:1 <addr> <file>` | YES | Load file from SD card |
| `md <addr> <count>` | YES | Display memory contents |
| `mw <addr> <value>` | YES | Write to memory |

---

## Troubleshooting

### Program hangs, doesn't return to U-Boot

**Cause:** Return address (LR) not saved properly

**Solution:** Check `start.S` has:
```assembly
push {r4-r11, lr}    /* At start */
pop {r4-r11, pc}     /* At end */
```

### No output on screen

**Cause:** Wrong UART address

**Solution:** 
1. Check your board's UART base address
2. In U-Boot, run `bdinfo` to find serial address
3. Update `UART_BASE` in `main.c`

### "Undefined instruction" error

**Cause:** Load address doesn't match linker address

**Solution:** 
1. Check address in `linker.ld`
2. Use same address in U-Boot `load` and `go` commands

### Build fails with "command not found"

**Cause:** Cross compiler not installed

**Solution:**
```bash
sudo apt-get install gcc-arm-linux-gnueabi
```

---

## Key Concepts Learned

### U-Boot Commands

| Command Type | Example | Returns? | Use Case |
|--------------|---------|----------|----------|
| Utility commands | `md`, `load`, `tftp` | YES | Debugging, loading files |
| Script execution | `run bootcmd` | YES | Run command sequences |
| Bare-metal jump | `go 0x60100000` | MAYBE | Test programs |
| OS boot | `bootz`, `bootm` | NEVER | Boot Linux |


## Project Structure

```
baremetal_app/
├── main.c          # Main program (UART printing)
├── start.S         # Entry point (saves/restores LR)
├── linker.ld       # Memory layout (load address)
├── Makefile        # Build rules
└── README.md       # This documentation
```

---

## Quick Reference

### Build and Run

```bash
# Build
make

# Copy to SD card (adjust mount point)
cp myprogram.bin /media/user/boot/

# In U-Boot
load mmc 0:1 0x60100000 myprogram.bin
go 0x60100000
```

### Find UART Address

```
# In U-Boot
=> bdinfo
...
serial addr = 0x10009000   ← Use this value
...
```

### Change Load Address

```
# 1. Edit linker.ld
. = 0x80000000;

# 2. Rebuild
make clean && make

# 3. Use new address in U-Boot
load mmc 0:1 0x80000000 myprogram.bin
go 0x80000000
```

---

## License

This code is provided as-is for educational purposes.
Free to use, modify, and distribute.

---
