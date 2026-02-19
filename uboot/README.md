# U-Boot Build and Deployment - README

## Part A: U-Boot Build and Deployment

---

## Table of Contents

1. [What is a Bootloader?](#1-what-is-a-bootloader)
2. [Boot Chain on Raspberry Pi and PC](#2-boot-chain-on-raspberry-pi-and-pc)
3. [Difference Between U-Boot and GRUB](#3-difference-between-u-boot-and-grub)
4. [Required Files for U-Boot on Raspberry Pi](#4-required-files-for-u-boot-on-raspberry-pi)
5. [Build and Test U-Boot in QEMU](#5-build-and-test-u-boot-in-qemu)

---

## 1. What is a Bootloader?

### Definition

A **bootloader** is the first software that runs when a device powers on. It initializes hardware and loads the operating system into memory.

### Why Do We Need It?

- CPU can only execute code from RAM
- RAM is empty when powered on
- OS is stored on disk/SD card
- Bootloader bridges this gap by copying OS to RAM

### Key Responsibilities

| Responsibility | Description |
|----------------|-------------|
| Hardware Init | Initialize CPU, memory, clocks |
| Memory Setup | Configure and test RAM |
| Load OS | Copy kernel from storage to RAM |
| Pass Control | Start the operating system |

### Bootloader Stages

```
Power On
    │
    ▼
┌─────────────────┐
│ Stage 0: ROM    │  Built into chip, loads Stage 1
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Stage 1: SPL    │  Initializes RAM, loads Stage 2
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Stage 2: U-Boot │  Full bootloader, loads OS
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Operating System│
└─────────────────┘
```

### Common Bootloaders

| Bootloader | Platform | Used In |
|------------|----------|---------|
| U-Boot | Embedded/ARM | Raspberry Pi, BeagleBone |
| GRUB | x86 PC | Linux desktops/servers |
| UEFI | Modern PC | Windows, Linux |

---

## 2. Boot Chain on Raspberry Pi and PC

### Raspberry Pi Boot Chain

**Important:** Raspberry Pi boots from GPU first, not CPU!

```
Power On
    │
    ▼
┌─────────────────────────────────────────────────────┐
│ Stage 0: Boot ROM (GPU)                             │
│ • Built into SoC chip                               │
│ • Loads bootcode.bin from SD card                   │
│ • ARM CPU is OFF                                    │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│ Stage 1: bootcode.bin (GPU)                         │
│ • Initializes SDRAM                                 │
│ • Loads start.elf                                   │
│ • ARM CPU is still OFF                              │
│ • Note: RPi 4 uses EEPROM instead                   │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│ Stage 2: start.elf (GPU)                            │
│ • Reads config.txt                                  │
│ • Loads Device Tree (.dtb)                          │
│ • Loads U-Boot (u-boot.bin)                         │
│ • Releases ARM CPU from reset                       │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│ Stage 3: U-Boot (ARM CPU)                           │
│ • ARM CPU now running!                              │
│ • Shows U-Boot prompt                               │
│ • Loads Linux kernel                                │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│ Linux Kernel → Init System → OS Ready               │
└─────────────────────────────────────────────────────┘
```

### Boot Files on SD Card

```
/boot/ (FAT32)
├── bootcode.bin      ← Stage 1 (RPi 3 and earlier)
├── start.elf         ← GPU firmware
├── fixup.dat         ← Memory configuration
├── config.txt        ← Boot configuration
├── u-boot.bin        ← U-Boot bootloader
├── bcm2711-rpi-4-b.dtb  ← Device Tree
└── overlays/         ← Device Tree overlays
```

---

### BONUS: PC Boot Chain

#### Legacy BIOS Boot

```
Power On
    │
    ▼
┌─────────────────────────────────────────────────────┐
│ BIOS (Basic Input/Output System)                    │
│ • POST (Power-On Self-Test)                         │
│ • Initialize hardware                               │
│ • Load MBR from disk                                │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│ MBR - GRUB Stage 1 (512 bytes)                      │
│ • First sector of disk                              │
│ • Loads GRUB Stage 1.5                              │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│ GRUB Stage 1.5                                      │
│ • Contains filesystem drivers                       │
│ • Loads GRUB Stage 2                                │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│ GRUB Stage 2                                        │
│ • Shows boot menu                                   │
│ • Loads Linux kernel                                │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│ Linux Kernel → Init System → OS Ready               │
└─────────────────────────────────────────────────────┘
```

#### UEFI Boot (Modern PCs)

```
Power On
    │
    ▼
┌─────────────────────────────────────────────────────┐
│ UEFI Firmware                                       │
│ • SEC → PEI → DXE → BDS phases                     │
│ • Initialize hardware                               │
│ • Find EFI System Partition (ESP)                   │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│ GRUB EFI (grubx64.efi)                              │
│ • Single stage bootloader                           │
│ • Shows boot menu                                   │
│ • Loads kernel                                      │
└────────────────────────┬────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────┐
│ Linux Kernel → Init System → OS Ready               │
└─────────────────────────────────────────────────────┘
```

### Comparison: RPi vs PC

| Aspect | Raspberry Pi | PC (BIOS) | PC (UEFI) |
|--------|--------------|-----------|-----------|
| First Processor | GPU | CPU | CPU |
| Boot ROM | GPU ROM | BIOS ROM | UEFI Firmware |
| Bootloader | U-Boot | GRUB | GRUB EFI |
| Config File | config.txt | grub.cfg | grub.cfg |
| HW Description | Device Tree | ACPI | ACPI |

---

## 3. Difference Between U-Boot and GRUB

### Quick Overview

| Feature | U-Boot | GRUB |
|---------|--------|------|
| **Target** | Embedded systems | Desktop/Server PCs |
| **Architecture** | ARM, MIPS, PowerPC, RISC-V | x86, x86_64 |
| **Interface** | Command-line (Serial) | Graphical menu |
| **Configuration** | Compile-time + env vars | grub.cfg file |
| **Hardware Info** | Device Tree (DTB) | ACPI tables |
| **Typical Device** | Raspberry Pi | Linux PC |

### Target Platforms

```
U-Boot                              GRUB
──────                              ────

┌─────────────────────┐             ┌─────────────────────┐
│  EMBEDDED SYSTEMS   │             │  DESKTOP/SERVER     │
├─────────────────────┤             ├─────────────────────┤
│ • Raspberry Pi      │             │ • Desktop PCs       │
│ • BeagleBone        │             │ • Laptops           │
│ • NVIDIA Jetson     │             │ • Servers           │
│ • Routers           │             │ • Virtual Machines  │
│ • IoT devices       │             │                     │
└─────────────────────┘             └─────────────────────┘
```

### User Interface

```
U-Boot Interface:                   GRUB Interface:
─────────────────                   ────────────────

┌─────────────────────┐             ┌─────────────────────┐
│ U-Boot 2023.10      │             │    GNU GRUB 2.06    │
│                     │             │                     │
│ Hit any key: 3      │             │  ► Ubuntu          │
│                     │             │    Ubuntu Recovery  │
│ U-Boot> _           │             │    Windows 10       │
│                     │             │                     │
│ (Command-line)      │             │  (Graphical menu)   │
└─────────────────────┘             └─────────────────────┘
```

### Configuration Method

| Aspect | U-Boot | GRUB |
|--------|--------|------|
| Config Type | Compile-time | Runtime file |
| Config Tool | menuconfig | /etc/default/grub |
| Config File | .config (build) | grub.cfg |
| Variables | Environment vars | grub.cfg entries |
| Update | Rebuild required | update-grub |

### When to Use Which?

**Use U-Boot when:**
- Working with embedded systems (ARM, MIPS)
- Using Raspberry Pi, BeagleBone
- Need network boot (TFTP)
- Developing bare-metal or RTOS

**Use GRUB when:**
- Standard x86/x86_64 PC
- Desktop Linux installation
- Dual-boot with Windows
- Need user-friendly boot menu

---

## 4. Required Files for U-Boot on Raspberry Pi

### Boot Partition Structure

```
/boot/ (FAT32 Partition)
│
├── bootcode.bin        [REQUIRED - RPi 3 and earlier]
├── start.elf           [REQUIRED - GPU firmware]
├── start4.elf          [REQUIRED - RPi 4 GPU firmware]
├── fixup.dat           [REQUIRED - Memory config]
├── fixup4.dat          [REQUIRED - RPi 4 memory config]
├── config.txt          [REQUIRED - Boot configuration]
├── bcm2711-rpi-4-b.dtb [REQUIRED - Device Tree]
├── u-boot.bin          [REQUIRED - U-Boot bootloader]
│
├── overlays/           [OPTIONAL - DT overlays]
├── boot.scr            [OPTIONAL - U-Boot script]
└── cmdline.txt         [OPTIONAL - Kernel parameters]
```

### File Descriptions

| File | Source | Purpose |
|------|--------|---------|
| bootcode.bin | RPi Foundation | Stage 1 bootloader (GPU), initializes RAM |
| start.elf | RPi Foundation | GPU firmware, loads U-Boot |
| fixup.dat | RPi Foundation | Memory configuration for GPU |
| config.txt | User created | Boot settings (kernel, UART, etc.) |
| *.dtb | RPi Foundation | Hardware description for kernel |
| u-boot.bin | Compiled | Main bootloader for ARM CPU |
| overlays/ | RPi Foundation | Hardware customization |
| boot.scr | User created | Automated boot commands |

### config.txt for U-Boot

```ini
# Enable 64-bit mode
arm_64bit=1

# Load U-Boot instead of kernel
kernel=u-boot.bin

# Enable serial console
enable_uart=1

# Disable Bluetooth (frees UART)
dtoverlay=disable-bt

# Minimal GPU memory
gpu_mem=16
```

### Where to Get Files

**Raspberry Pi Firmware:**
```bash
git clone --depth 1 https://github.com/raspberrypi/firmware
cp firmware/boot/bootcode.bin /boot/
cp firmware/boot/start*.elf /boot/
cp firmware/boot/fixup*.dat /boot/
cp firmware/boot/*.dtb /boot/
```

**U-Boot:**
```bash
git clone https://github.com/u-boot/u-boot
cd u-boot
make rpi_4_defconfig
make CROSS_COMPILE=aarch64-linux-gnu- -j4
cp u-boot.bin /boot/
```

---

## 5. Build and Test U-Boot in QEMU

### Part A: Build U-Boot with Custom Configuration

#### Step 1: Install Prerequisites

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    gcc-arm-linux-gnueabi \
    bison \
    flex \
    libssl-dev \
    libncurses5-dev \
    qemu-system-arm \
    git
```

#### Step 2: Download U-Boot

```bash
mkdir ~/uboot-vexpress
cd ~/uboot-vexpress
git clone https://github.com/u-boot/u-boot.git
cd u-boot
git checkout v2023.10
```

#### Step 3: Apply Default Configuration

```bash
make vexpress_ca9x4_defconfig
```

#### Step 4: Customize with menuconfig

```bash
make menuconfig
```

**Configuration Changes Made:**

| Location | Setting | Value |
|----------|---------|-------|
| Command line interface | Shell prompt | `Gemy=>` |
| Environment | Storage type | FAT filesystem |
| Environment | Block device name | `mmc` |
| Environment | Device and partition | `0:1` |
| Boot options | bootcmd value | `echo hello gemy` |

**Change 1: Shell Prompt**
```
Path: Command line interface → Shell prompt
Value: Gemy=>
```

**Change 2: Environment Storage**
```
Path: Environment → Select environment storage
Value: Environment is in a FAT filesystem
```

**Change 3: FAT Device Settings**
```
Path: Environment → Name of block device
Value: mmc

Path: Environment → Device and partition
Value: 0:1
```

**Change 4: Boot Command**
```
Path: Boot options → bootcmd value
Value: echo hello gemy
```

#### Step 5: Save and Exit

- Press `ESC ESC`
- Select `Yes` to save

#### Step 6: Set Cross Compiler and Build

```bash
export CROSS_COMPILE=arm-linux-gnueabi-
make -j4
```

**Output files:**
- `u-boot` - ELF executable
- `u-boot.bin` - Raw binary

---

### Part B: Run U-Boot in QEMU

#### QEMU Command

```bash
qemu-system-arm -M vexpress-a9 -kernel u-boot -nographic
```

#### Command Explanation

| Option | Description |
|--------|-------------|
| `qemu-system-arm` | QEMU emulator for ARM |
| `-M vexpress-a9` | Machine: Versatile Express Cortex-A9 |
| `-kernel u-boot` | Load U-Boot ELF file |
| `-nographic` | No GUI, output to terminal |

#### Expected Output

```
U-Boot 2023.10 (Oct 20 2023 - 10:30:00 +0000)

DRAM:  128 MiB
Flash: 128 MiB
MMC:   MMC: 0
Loading Environment from FAT... OK
In:    serial
Out:   serial
Err:   serial
Net:   smc911x-0
Hit any key to stop autoboot:  3  2  1  0
hello gemy                    ← bootcmd executed
Gemy=>                        ← Custom prompt
```

#### Test Commands

```bash
# Show version
Gemy=> version

# Show environment
Gemy=> printenv

# Run bootcmd
Gemy=> run bootcmd
hello gemy

# Show board info
Gemy=> bdinfo

# Help
Gemy=> help
```

#### Exit QEMU

```
Press: Ctrl+A then X
```

---

## Quick Reference: All Commands

```bash
# === STEP 1: Install tools ===
sudo apt install build-essential gcc-arm-linux-gnueabi \
    bison flex libssl-dev libncurses5-dev qemu-system-arm git

# === STEP 2: Get U-Boot ===
mkdir ~/uboot-vexpress && cd ~/uboot-vexpress
git clone https://github.com/u-boot/u-boot.git
cd u-boot

# === STEP 3: Configure ===
make vexpress_ca9x4_defconfig
make menuconfig    # Make changes as described above

# === STEP 4: Build ===
export CROSS_COMPILE=arm-linux-gnueabi-
make -j4

# === STEP 5: Run in QEMU ===
qemu-system-arm -M vexpress-a9 -kernel u-boot -nographic

# === STEP 6: Exit QEMU ===
# Press: Ctrl+A then X
```

---

