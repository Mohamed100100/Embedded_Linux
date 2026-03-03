# U-Boot Build and Deployment - README

## Part A: U-Boot Build and Deployment

---

## Table of Contents

1. [What is a Bootloader?](#1-what-is-a-bootloader)
2. [Boot Chain on Raspberry Pi and PC](#2-boot-chain-on-raspberry-pi-and-pc)
3. [Difference Between U-Boot and GRUB](#3-difference-between-u-boot-and-grub)
4. [Required Files for U-Boot on Raspberry Pi](#4-required-files-for-u-boot-on-raspberry-pi)
5. [Build and Test U-Boot in QEMU](#5-build-and-test-u-boot-in-qemu)
6. [Hardware Description File on Raspberry Pi](https://playground.outlier.ai/conversation/69a648c9e86a8d9419d65cea#6-hardware-description-file-on-raspberry-pi)
7. [Loop Device Partition Detection](https://playground.outlier.ai/conversation/69a648c9e86a8d9419d65cea#7-loop-device-partition-detection)

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
## Part A: U-Boot Build and Deployment (Continued)

---

## Table of Contents - Part A (Continued)

6. [Hardware Description File on Raspberry Pi](#6-hardware-description-file-on-raspberry-pi)
7. [Loop Device Partition Detection](#7-loop-device-partition-detection)

---

## 6. Hardware Description File on Raspberry Pi

### Question 7: Which file provides the hardware description to U-Boot on the Raspberry Pi 3B+ and at which stage is it loaded?

### Answer

The **Device Tree Blob (DTB)** file provides hardware description to U-Boot on the Raspberry Pi 3B+.

### Specific File for RPi 3B+

```
bcm2837-rpi-3-b-plus.dtb
```

### Loading Stage

```
┌─────────────────────────────────────────────────────────────────┐
│                    DEVICE TREE LOADING STAGE                     │
└─────────────────────────────────────────────────────────────────┘

Power On
    │
    ▼
┌─────────────────────────────────────────────────────────────────┐
│ Stage 0: Boot ROM (GPU)                                         │
│ • DTB NOT loaded yet                                            │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────────┐
│ Stage 1: bootcode.bin (GPU)                                     │
│ • DTB NOT loaded yet                                            │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────────┐
│ Stage 2: start.elf (GPU)  ◄─── DTB LOADED HERE!                │
│                                                                 │
│ • Reads config.txt                                              │
│ • Loads bcm2837-rpi-3-b-plus.dtb from SD card                  │
│ • Applies overlays from /overlays directory                     │
│ • Places DTB in memory at specific address                      │
│ • Passes DTB address to U-Boot                                  │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────────┐
│ Stage 3: U-Boot (ARM CPU)                                       │
│ • Receives DTB address from start.elf                           │
│ • Uses DTB for hardware information                             │
│ • Passes DTB to Linux kernel                                    │
└─────────────────────────────────────────────────────────────────┘
```

### What is a Device Tree?

| Aspect          | Description                                          |
| --------------- | ---------------------------------------------------- |
| **Purpose**     | Describes hardware layout to software                |
| **Format**      | .dts (source) → .dtb (compiled binary)               |
| **Contains**    | CPU info, memory map, peripherals, interrupts, GPIOs |
| **Who Uses It** | U-Boot and Linux Kernel                              |

### Device Tree Files for Raspberry Pi Family

| Model             | DTB File                   |
| ----------------- | -------------------------- |
| RPi 3B            | bcm2837-rpi-3-b.dtb        |
| **RPi 3B+**       | **bcm2837-rpi-3-b-plus.dtb** |
| RPi 4B            | bcm2711-rpi-4-b.dtb        |
| RPi Zero W        | bcm2835-rpi-zero-w.dtb     |
| RPi CM4           | bcm2711-rpi-cm4.dtb        |

### Example Device Tree Structure

```dts
/dts-v1/;

/ {
    compatible = "raspberrypi,3-model-b-plus", "brcm,bcm2837";
    model = "Raspberry Pi 3 Model B+";

    memory@0 {
        device_type = "memory";
        reg = <0x0 0x40000000>;  /* 1GB RAM */
    };

    soc {
        compatible = "simple-bus";

        gpio: gpio@7e200000 {
            compatible = "brcm,bcm2835-gpio";
            reg = <0x7e200000 0xb4>;
        };

        uart0: serial@7e201000 {
            compatible = "arm,pl011";
            reg = <0x7e201000 0x200>;
        };
    };
};
```

### How start.elf Selects the DTB

```ini
# config.txt can specify DTB manually
device_tree=bcm2837-rpi-3-b-plus.dtb

# Or auto-select based on hardware (default behavior)
# start.elf detects board revision and loads appropriate DTB
```

### Memory Location of DTB

```
┌────────────────────────────────────────────────────────────┐
│                    RAM MEMORY MAP                           │
├────────────────────────────────────────────────────────────┤
│ 0x00000000 ┌──────────────────────────────────────────┐    │
│            │ ARM Exception Vectors                    │    │
│ 0x00008000 ├──────────────────────────────────────────┤    │
│            │ Linux Kernel (zImage)                    │    │
│            │                                          │    │
│ 0x02000000 ├──────────────────────────────────────────┤    │
│            │ Device Tree Blob (DTB)  ◄── Loaded here │    │
│            │                                          │    │
│ 0x02100000 ├──────────────────────────────────────────┤    │
│            │ Initramfs (optional)                     │    │
│            │                                          │    │
└────────────┴──────────────────────────────────────────┴────┘
```

---

## 7. Loop Device Partition Detection

### Question 8: After `losetup --partscan --show -f sd.img` we get devices like /dev/loop5p1 and /dev/loop5p2. Explain how the Linux kernel knows where the partitions start inside the image file.

### Answer

The Linux kernel reads the **partition table** stored at the beginning of the image file to determine partition locations.

### The `--partscan` Option

```bash
losetup --partscan --show -f sd.img
```

| Option       | Description                                    |
| ------------ | ---------------------------------------------- |
| `--partscan` | Tell kernel to scan for partitions             |
| `--show`     | Print the loop device name                     |
| `-f`         | Find first available loop device               |
| `sd.img`     | The disk image file                            |

### How Partition Detection Works

```
┌────────────────────────────────────────────────────────────────────┐
│                     PARTITION DETECTION FLOW                        │
└────────────────────────────────────────────────────────────────────┘

┌──────────────┐     ┌──────────────┐     ┌──────────────────────────┐
│   sd.img     │────►│   losetup    │────►│  Kernel Loop Driver      │
│   (file)     │     │  --partscan  │     │  /dev/loop5              │
└──────────────┘     └──────────────┘     └────────────┬─────────────┘
                                                       │
                                                       ▼
                                          ┌──────────────────────────┐
                                          │  Kernel reads first      │
                                          │  512 bytes (MBR) or      │
                                          │  GPT header              │
                                          └────────────┬─────────────┘
                                                       │
                                                       ▼
                                          ┌──────────────────────────┐
                                          │  Parse partition table   │
                                          │  • Start sector          │
                                          │  • Size                  │
                                          │  • Type                  │
                                          └────────────┬─────────────┘
                                                       │
                                                       ▼
                                          ┌──────────────────────────┐
                                          │  Create partition nodes  │
                                          │  • /dev/loop5p1          │
                                          │  • /dev/loop5p2          │
                                          └──────────────────────────┘
```

### MBR Partition Table Structure

The **Master Boot Record (MBR)** is located at byte offset 0 of the disk/image:

```
┌────────────────────────────────────────────────────────────────────┐
│                    MBR STRUCTURE (512 bytes)                        │
├────────────────────────────────────────────────────────────────────┤
│ Offset    │ Size    │ Description                                  │
├───────────┼─────────┼──────────────────────────────────────────────┤
│ 0x000     │ 446     │ Bootstrap code (bootloader)                  │
│ 0x1BE     │ 16      │ Partition Entry 1 ◄──────────────────────────│
│ 0x1CE     │ 16      │ Partition Entry 2 ◄──────────────────────────│
│ 0x1DE     │ 16      │ Partition Entry 3                            │
│ 0x1EE     │ 16      │ Partition Entry 4                            │
│ 0x1FE     │ 2       │ Boot Signature (0x55AA)                      │
└───────────┴─────────┴──────────────────────────────────────────────┘
```

### Partition Entry Structure (16 bytes each)

```
┌────────────────────────────────────────────────────────────────────┐
│              PARTITION ENTRY STRUCTURE (16 bytes)                   │
├────────────────────────────────────────────────────────────────────┤
│ Offset │ Size │ Field                    │ Example                 │
├────────┼──────┼──────────────────────────┼─────────────────────────┤
│ 0x00   │ 1    │ Boot flag (0x80=active)  │ 0x00                    │
│ 0x01   │ 3    │ CHS start address        │ (legacy, ignored)       │
│ 0x04   │ 1    │ Partition type           │ 0x0C (FAT32 LBA)        │
│ 0x05   │ 3    │ CHS end address          │ (legacy, ignored)       │
│ 0x08   │ 4    │ LBA start sector ◄───────│ 2048 (0x00000800)       │
│ 0x0C   │ 4    │ Number of sectors        │ 131072 (64MB)           │
└────────┴──────┴──────────────────────────┴─────────────────────────┘
```

### Example: Reading Partition Table

```bash
# View raw partition table with hexdump
hexdump -C -s 446 -n 66 sd.img
```

**Example output:**
```
000001be  00 20 21 00 0c fe ff ff  00 08 00 00 00 00 02 00  |. !.............|
000001ce  00 fe ff ff 83 fe ff ff  00 08 02 00 00 00 1e 00  |................|
000001de  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
000001ee  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
000001fe  55 aa                                             |U.|
```

### Decoding the Example

```
Partition 1:
├── Start LBA: 0x00000800 = 2048 sectors = 2048 × 512 = 1,048,576 bytes = 1MB
├── Size: 0x00020000 = 131,072 sectors = 64MB
└── Type: 0x0C = FAT32 LBA

Partition 2:
├── Start LBA: 0x00020800 = 133,120 sectors = 68,157,440 bytes ≈ 65MB
├── Size: 0x001E0000 = 1,966,080 sectors ≈ 960MB
└── Type: 0x83 = Linux native (ext4)
```

### Visual Representation

```
┌────────────────────────────────────────────────────────────────────┐
│                        sd.img FILE LAYOUT                           │
└────────────────────────────────────────────────────────────────────┘

Byte Offset:
0          512        1MB                      65MB               1GB
│          │          │                        │                  │
▼          ▼          ▼                        ▼                  ▼
┌──────────┬──────────┬────────────────────────┬──────────────────┐
│   MBR    │  (gap)   │     Partition 1        │   Partition 2    │
│ 512bytes │          │     (FAT32 - boot)     │   (ext4 - root)  │
│          │          │     /dev/loop5p1       │   /dev/loop5p2   │
└──────────┴──────────┴────────────────────────┴──────────────────┘
           │          │                        │
           │          └── LBA 2048             └── LBA 133120
           │              (from partition       (from partition
           │               table entry 1)        table entry 2)
           │
           └── Partition table at offset 446
```

### Kernel Code Path

```
1. losetup creates /dev/loop5 → associates with sd.img
                    │
                    ▼
2. --partscan triggers: loop_reread_partitions()
                    │
                    ▼
3. Kernel calls: blkdev_reread_part()
                    │
                    ▼
4. Partition parser: msdos_partition() or efi_partition()
                    │
                    ▼
5. For each partition found:
   └── Creates /dev/loop5p1, /dev/loop5p2, etc.
```

### GPT vs MBR

| Feature          | MBR                    | GPT                        |
| ---------------- | ---------------------- | -------------------------- |
| Location         | First 512 bytes        | First 34 sectors           |
| Max Partitions   | 4 primary              | 128                        |
| Max Disk Size    | 2TB                    | 9.4 ZB                     |
| Partition Info   | Offset 446, 64 bytes   | LBA 2-33                   |
| Signature        | 0x55AA at 510          | "EFI PART" at LBA 1        |

### Verify Partitions Created

```bash
# After losetup
losetup --partscan --show -f sd.img
# Output: /dev/loop5

# Check partitions
lsblk /dev/loop5
# Output:
# NAME      SIZE TYPE MOUNTPOINT
# loop5     1G  loop 
# ├─loop5p1 64M part 
# └─loop5p2 960M part

# View partition details
fdisk -l /dev/loop5
```

---

# Part B: U-Boot Commands & Environment

---

## Table of Contents - Part B

1. [bdinfo Command](#1-bdinfo-command)
2. [printenv Command](#2-printenv-command)
3. [DRAM Start Address](#3-dram-start-address)
4. [List and Load Files from FAT Partition](#4-list-and-load-files-from-fat-partition)
5. [Customize U-Boot Banner](#5-customize-u-boot-banner)
6. [Add Custom Command](#6-add-custom-command)
7. [Network Booting with TFTP](#7-network-booting-with-tftp)
8. [Difference Between run and go Commands](#8-difference-between-run-and-go-commands)
9. [Purpose of bootargs](#9-purpose-of-bootargs)
10. [Kernel Address Selection](#10-kernel-address-selection)

---

## 1. bdinfo Command

### Question: What is the use of "bdinfo" command?

### Answer

The `bdinfo` command displays **Board Information** - comprehensive details about the hardware platform U-Boot is running on.

### Usage

```bash
U-Boot> bdinfo
```

### Example Output (Vexpress-A9 in QEMU)

```
arch_number = 0x000008e0
boot_params = 0x60000100
DRAM bank   = 0x00000000
-> start    = 0x60000000
-> size     = 0x08000000
baudrate    = 115200 bps
TLB addr    = 0x67ff0000
relocaddr   = 0x67f70000
reloc off   = 0x07f70000
irq_sp      = 0x67ef6ea0
sp start    = 0x67ef6e90
arm frequency = 400 MHz
```

### Information Provided by bdinfo

| Field            | Description                              | Example Value  |
| ---------------- | ---------------------------------------- | -------------- |
| `arch_number`    | Machine type ID                          | 0x8e0          |
| `boot_params`    | Address for kernel boot parameters       | 0x60000100     |
| `DRAM bank`      | Memory bank number                       | 0              |
| `-> start`       | **DRAM start address**                   | 0x60000000     |
| `-> size`        | Total DRAM size                          | 0x08000000 (128MB) |
| `baudrate`       | Serial console baud rate                 | 115200         |
| `TLB addr`       | Translation Lookaside Buffer address     | 0x67ff0000     |
| `relocaddr`      | U-Boot relocation address                | 0x67f70000     |
| `reloc off`      | Relocation offset                        | 0x07f70000     |
| `sp start`       | Stack pointer start                      | 0x67ef6e90     |

### Use Cases

```
┌────────────────────────────────────────────────────────────────────┐
│                    BDINFO USE CASES                                 │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│  1. Finding memory addresses for loading kernel/DTB                │
│     └── Need to know DRAM start and size                          │
│                                                                    │
│  2. Debugging boot issues                                          │
│     └── Verify hardware is detected correctly                      │
│                                                                    │
│  3. Checking serial port configuration                             │
│     └── Verify baudrate matches terminal                           │
│                                                                    │
│  4. Understanding memory layout                                    │
│     └── See where U-Boot relocated itself                          │
│                                                                    │
└────────────────────────────────────────────────────────────────────┘
```

---

## 2. printenv Command

### Question: What is the use of "printenv" command?

### Answer

The `printenv` command displays **U-Boot environment variables** that control boot behavior, network settings, and system configuration.

### Usage

```bash
# Print all environment variables
U-Boot> printenv

# Print specific variable
U-Boot> printenv bootcmd

# Print multiple variables
U-Boot> printenv bootargs serverip
```

### Example Output

```
U-Boot> printenv
baudrate=115200
bootargs=console=ttyAMA0 root=/dev/mmcblk0p2 rootwait
bootcmd=echo hello gemy
bootdelay=3
ethaddr=52:54:00:12:34:56
fdtfile=vexpress-v2p-ca9.dtb
ipaddr=192.168.1.100
kernel_addr_r=0x60008000
loadaddr=0x60000000
serverip=192.168.1.1
stderr=serial
stdin=serial
stdout=serial
```

### Important Environment Variables

| Variable       | Purpose                              | Example                          |
| -------------- | ------------------------------------ | -------------------------------- |
| `bootcmd`      | Auto-executed boot command           | `run distro_bootcmd`             |
| `bootargs`     | Kernel command line                  | `console=ttyS0 root=/dev/sda1`   |
| `bootdelay`    | Seconds before auto-boot             | `3`                              |
| `loadaddr`     | Default load address                 | `0x60000000`                     |
| `kernel_addr_r`| Where to load kernel                 | `0x60008000`                     |
| `fdt_addr_r`   | Where to load device tree            | `0x65000000`                     |
| `ipaddr`       | Board IP address                     | `192.168.1.100`                  |
| `serverip`     | TFTP server IP                       | `192.168.1.1`                    |
| `ethaddr`      | MAC address                          | `52:54:00:12:34:56`              |

### Environment Commands

```bash
# Print all variables
printenv

# Set a variable
setenv myvar "hello world"

# Delete a variable
setenv myvar

# Save to persistent storage
saveenv

# Reset to default
env default -a
```

### Visual: Environment Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                ENVIRONMENT VARIABLE FLOW                         │
└─────────────────────────────────────────────────────────────────┘

┌──────────────────┐     ┌──────────────────┐     ┌──────────────┐
│  Default Env     │────►│  Stored Env      │────►│  Runtime Env │
│  (compiled in)   │     │  (SD/Flash/MMC)  │     │  (in RAM)    │
└──────────────────┘     └──────────────────┘     └──────────────┘
                                  │                      │
                                  │ saveenv              │ setenv
                                  ◄──────────────────────┘
```

---

## 3. DRAM Start Address

### Question: What is the DRAM start address?

### Answer

The DRAM start address depends on the platform. Use `bdinfo` to find it.

### Common DRAM Start Addresses

| Platform               | DRAM Start Address | How to Find           |
| ---------------------- | ------------------ | --------------------- |
| Vexpress-A9 (QEMU)     | **0x60000000**     | `bdinfo`              |
| Raspberry Pi 3/4       | **0x00000000**     | `bdinfo`              |
| BeagleBone Black       | **0x80000000**     | `bdinfo`              |
| i.MX6                  | **0x10000000**     | `bdinfo`              |

### Finding DRAM Address in QEMU (Vexpress-A9)

```bash
U-Boot> bdinfo
...
DRAM bank   = 0x00000000
-> start    = 0x60000000    ◄── DRAM START ADDRESS
-> size     = 0x08000000    ◄── 128MB DRAM SIZE
...
```

### Memory Map Visualization (Vexpress-A9)

```
┌────────────────────────────────────────────────────────────────────┐
│                VEXPRESS-A9 MEMORY MAP                               │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│  0x00000000 ┌────────────────────────────┐                        │
│             │    I/O Peripherals         │                        │
│             │    (not DRAM)              │                        │
│  0x60000000 ├────────────────────────────┤ ◄── DRAM START         │
│             │                            │                        │
│             │    DRAM (128MB)            │                        │
│             │                            │                        │
│             │  0x60008000 - Kernel load  │                        │
│             │  0x65000000 - DTB load     │                        │
│             │  0x66000000 - Initrd load  │                        │
│             │                            │                        │
│  0x68000000 └────────────────────────────┘ ◄── DRAM END           │
│                                                                    │
└────────────────────────────────────────────────────────────────────┘
```

### Using DRAM Address

```bash
# Load kernel at DRAM start + offset
U-Boot> load mmc 0:1 0x60008000 zImage

# Load device tree
U-Boot> load mmc 0:1 0x65000000 vexpress-v2p-ca9.dtb

# Boot
U-Boot> bootz 0x60008000 - 0x65000000
```

---

## 4. List and Load Files from FAT Partition

### Question: List and Load Files from FAT Partition

### Answer

### Step 1: Create SD Card Image with FAT Partition

```bash
# Create 64MB image
dd if=/dev/zero of=sd.img bs=1M count=64

# Create partition table
cat << EOF | fdisk sd.img
o
n
p
1


t
c
w
EOF

# Setup loop device
sudo losetup --partscan --show -f sd.img
# Output: /dev/loop0

# Format as FAT32
sudo mkfs.vfat -F 32 /dev/loop0p1

# Mount and add files
sudo mkdir -p /mnt/boot
sudo mount /dev/loop0p1 /mnt/boot

# Create test files
echo "Hello from file1" | sudo tee /mnt/boot/test1.txt
echo "Hello from file2" | sudo tee /mnt/boot/test2.txt
echo "Kernel placeholder" | sudo tee /mnt/boot/zImage

# Unmount
sudo umount /mnt/boot
```

### Step 2: Run QEMU with SD Card

```bash
qemu-system-arm \
    -M vexpress-a9 \
    -kernel u-boot \
    -nographic \
    -sd sd.img
```

### Step 3: U-Boot Commands for FAT

#### List Files

```bash
# List files on FAT partition (device 0, partition 1)
U-Boot> fatls mmc 0:1
```

**Output:**
```
    17   test1.txt
    17   test2.txt
    19   zImage

3 file(s), 0 dir(s)
```

#### Load File to Memory

```bash
# Load file to specific address
U-Boot> fatload mmc 0:1 0x60000000 test1.txt

# Verify with memory dump
U-Boot> md.b 0x60000000 20
```

**Output:**
```
60000000: 48 65 6c 6c 6f 20 66 72 6f 6d 20 66 69 6c 65 31    Hello from file1
60000010: 0a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
```

### FAT Commands Summary

| Command                              | Description                        |
| ------------------------------------ | ---------------------------------- |
| `fatls mmc 0:1`                      | List files on MMC device 0, part 1 |
| `fatls mmc 0:1 /folder`              | List files in subdirectory         |
| `fatload mmc 0:1 <addr> <file>`      | Load file to memory address        |
| `fatsize mmc 0:1 <file>`             | Get file size                      |
| `fatinfo mmc 0:1`                    | Show FAT filesystem info           |
| `fatwrite mmc 0:1 <addr> <file> <size>` | Write memory to file            |

### Example: Loading Kernel and DTB

```bash
# Load kernel
U-Boot> fatload mmc 0:1 0x60008000 zImage
# Output: 4000000 bytes read in 250 ms

# Load device tree
U-Boot> fatload mmc 0:1 0x65000000 vexpress-v2p-ca9.dtb
# Output: 14692 bytes read in 10 ms

# Check loaded files
U-Boot> iminfo 0x60008000
```

### Visual: FAT File Loading

```
┌────────────────────────────────────────────────────────────────────┐
│                    FAT FILE LOADING PROCESS                         │
└────────────────────────────────────────────────────────────────────┘

┌──────────────────┐          ┌──────────────────┐
│    SD Card       │          │      DRAM        │
│   (FAT32)        │          │                  │
├──────────────────┤          ├──────────────────┤
│                  │          │                  │
│  zImage          │ ────────►│  0x60008000      │
│  (4MB)           │ fatload  │  [kernel image]  │
│                  │          │                  │
│  board.dtb       │ ────────►│  0x65000000      │
│  (16KB)          │ fatload  │  [device tree]   │
│                  │          │                  │
│  boot.scr        │          │  0x66000000      │
│  test.txt        │          │  [initramfs]     │
│                  │          │                  │
└──────────────────┘          └──────────────────┘
```

---

## 5. Customize U-Boot Banner

### Question: Make the U-Boot banner say "Welcome to Our-Boot – Intake 46"

### Answer

### Method 1: Using menuconfig

```bash
cd u-boot
make menuconfig
```

**Navigate to:**
```
Command line interface
    └── Banner
        └── Boot banner
            └── Change to: "Welcome to Our-Boot - Intake 46"
```

### Method 2: Edit Source Code

**File:** `include/configs/vexpress_common.h` (or board-specific config)

```c
#define CONFIG_SYS_BANNER "Welcome to Our-Boot - Intake 46"
```

**Or edit:** `common/board_r.c`

```c
static int initr_announce(void)
{
    printf("\n\n");
    printf("=========================================\n");
    printf("  Welcome to Our-Boot - Intake 46\n");
    printf("=========================================\n");
    printf("\n");
    return 0;
}
```

### Method 3: Custom Version String

**Edit:** `include/version.h` or use Kconfig

**In menuconfig:**
```
General setup
    └── Local version - append to U-Boot release
        └── Enter: "-Intake46"
    
    └── Custom version string
        └── Enter: "Welcome to Our-Boot - Intake 46"
```

### Method 4: Modify Display Banner Function

**File:** `common/board_f.c`

```c
static int display_text_info(void)
{
    printf("\n");
    printf("==========================================\n");
    printf("  Welcome to Our-Boot - Intake 46\n");  
    printf("==========================================\n");
    printf("\n");
    
    /* Original banner code continues... */
    return 0;
}
```

### Build and Test

```bash
# Clean previous build
make clean

# Apply config
make vexpress_ca9x4_defconfig

# Make changes
make menuconfig

# Build
export CROSS_COMPILE=arm-linux-gnueabi-
make -j4

# Test in QEMU
qemu-system-arm -M vexpress-a9 -kernel u-boot -nographic
```

### Expected Output

```
==========================================
  Welcome to Our-Boot - Intake 46
==========================================

DRAM:  128 MiB
Flash: 128 MiB
...
```

---

## 6. Add Custom Command

### Question: Add a custom command "hello" that prints your name

### Answer

### Step 1: Create Command Source File

**Create file:** `cmd/cmd_hello.c`

```c
/*
 * cmd_hello.c - Custom hello command for U-Boot
 * Prints a personalized greeting message
 */

#include <common.h>
#include <command.h>

static int do_hello(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    printf("\n");
    printf("========================================\n");
    printf("  Hello! My name is Gemy               \n");
    printf("  Welcome to U-Boot Custom Commands    \n");
    printf("  Embedded Linux - Intake 46           \n");
    printf("========================================\n");
    printf("\n");
    
    /* If argument provided, greet that name */
    if (argc > 1) {
        printf("  Hello, %s! Nice to meet you!\n\n", argv[1]);
    }
    
    return 0;
}

U_BOOT_CMD(
    hello,      /* Command name */
    2,          /* Max arguments (cmd + 1 arg) */
    1,          /* Repeatable? 1=yes */
    do_hello,   /* Function to call */
    "Print hello message with my name",  /* Short description */
    "[name]\n"  /* Long description / usage */
    "    - Print greeting message\n"
    "    - Optional: provide a name to greet"
);
```

### Step 2: Add to Makefile

**Edit:** `cmd/Makefile`

Add this line:
```makefile
obj-y += cmd_hello.o
```

**Or for conditional compilation:**
```makefile
obj-$(CONFIG_CMD_HELLO) += cmd_hello.o
```

### Step 3: Add Kconfig Entry (Optional)

**Edit:** `cmd/Kconfig`

```kconfig
config CMD_HELLO
    bool "hello command"
    default y
    help
      Print a custom hello message with your name.
      Usage: hello [name]
```

### Step 4: Enable in menuconfig (if using Kconfig)

```bash
make menuconfig
```

**Navigate to:**
```
Command line interface
    └── Info commands
        └── [*] hello command
```

### Step 5: Build and Test

```bash
# Clean and rebuild
make clean
make vexpress_ca9x4_defconfig
export CROSS_COMPILE=arm-linux-gnueabi-
make -j4

# Run in QEMU
qemu-system-arm -M vexpress-a9 -kernel u-boot -nographic
```

### Step 6: Test the Command

```bash
# Basic usage
U-Boot> hello
========================================
  Hello! My name is Gemy
  Welcome to U-Boot Custom Commands
  Embedded Linux - Intake 46
========================================

# With argument
U-Boot> hello Ahmed
========================================
  Hello! My name is Gemy
  Welcome to U-Boot Custom Commands
  Embedded Linux - Intake 46
========================================
  Hello, Ahmed! Nice to meet you!

# Get help
U-Boot> help hello
hello - Print hello message with my name

Usage:
hello [name]
    - Print greeting message
    - Optional: provide a name to greet
```

### Understanding U_BOOT_CMD Macro

```c
U_BOOT_CMD(
    name,       /* Command name - what user types */
    maxargs,    /* Maximum number of arguments */
    repeatable, /* Can repeat with Enter key? */
    function,   /* C function to execute */
    shorthelp,  /* Brief description for 'help' */
    longhelp    /* Detailed usage for 'help <cmd>' */
);
```

### Visual: Command Registration

```
┌────────────────────────────────────────────────────────────────────┐
│                    CUSTOM COMMAND FLOW                              │
└────────────────────────────────────────────────────────────────────┘

┌──────────────────┐     ┌──────────────────┐     ┌──────────────────┐
│  cmd_hello.c     │────►│  U_BOOT_CMD      │────►│  Command Table   │
│                  │     │  macro           │     │  (linked list)   │
└──────────────────┘     └──────────────────┘     └────────┬─────────┘
                                                           │
                         ┌─────────────────────────────────┘
                         │
                         ▼
┌──────────────────────────────────────────────────────────────────┐
│  User types: "hello"                                             │
│       │                                                          │
│       ▼                                                          │
│  U-Boot searches command table                                   │
│       │                                                          │
│       ▼                                                          │
│  Finds "hello" → calls do_hello()                                │
│       │                                                          │
│       ▼                                                          │
│  do_hello() executes → prints output                             │
└──────────────────────────────────────────────────────────────────┘
```

---

## 7. Network Booting with TFTP

### Question: Network Booting with TFTP

### Answer

### Part A: Set Up TFTP Server on Your Laptop

#### Step 1: Install TFTP Server

```bash
# For Ubuntu/Debian
sudo apt update
sudo apt install -y tftpd-hpa tftp-hpa

# Verify installation
systemctl status tftpd-hpa
```

#### Step 2: Configure TFTP Server

**Edit:** `/etc/default/tftpd-hpa`

```bash
# TFTP directory
TFTP_USERNAME="tftp"
TFTP_DIRECTORY="/srv/tftp"
TFTP_ADDRESS=":69"
TFTP_OPTIONS="--secure --create"
```

#### Step 3: Setup TFTP Directory

```bash
# Create directory
sudo mkdir -p /srv/tftp

# Set permissions
sudo chown -R tftp:tftp /srv/tftp
sudo chmod -R 755 /srv/tftp

# Restart service
sudo systemctl restart tftpd-hpa
sudo systemctl enable tftpd-hpa
```

#### Step 4: Add Boot Files

```bash
# Copy kernel (example)
sudo cp zImage /srv/tftp/

# Copy device tree
sudo cp vexpress-v2p-ca9.dtb /srv/tftp/

# Create a test file
echo "TFTP Test File" | sudo tee /srv/tftp/test.txt

# Verify files
ls -la /srv/tftp/
```

#### Step 5: Configure Firewall

```bash
# Allow TFTP port
sudo ufw allow 69/udp
sudo ufw reload
```

#### Step 6: Test TFTP Server

```bash
# From same machine
cd /tmp
tftp localhost
tftp> get test.txt
tftp> quit
cat test.txt
# Should show: TFTP Test File
```

### Network Diagram

```
┌────────────────────────────────────────────────────────────────────┐
│                      TFTP BOOT SETUP                                │
└────────────────────────────────────────────────────────────────────┘

     ┌─────────────────┐                    ┌─────────────────┐
     │   TFTP Server   │                    │   Target Board  │
     │   (Laptop)      │                    │   (RPi/QEMU)    │
     │                 │                    │                 │
     │  IP: 192.168.1.1│◄──────────────────►│ IP: 192.168.1.10│
     │  Port: 69/UDP   │     Network        │                 │
     │                 │                    │  U-Boot         │
     │  /srv/tftp/     │                    │                 │
     │  ├── zImage     │────── TFTP ───────►│  0x60008000     │
     │  ├── board.dtb  │────── TFTP ───────►│  0x65000000     │
     │  └── initrd     │────── TFTP ───────►│  0x66000000     │
     └─────────────────┘                    └─────────────────┘
```

---

### Part B: Configure Network & Test from U-Boot

#### For QEMU: Run with Network

```bash
# Create TAP interface (requires root)
sudo ip tuntap add tap0 mode tap user $USER
sudo ip addr add 192.168.1.1/24 dev tap0
sudo ip link set tap0 up

# Run QEMU with network
qemu-system-arm \
    -M vexpress-a9 \
    -kernel u-boot \
    -nographic \
    -net nic \
    -net tap,ifname=tap0,script=no,downscript=no
```

**Alternative: User-mode networking (simpler)**
```bash
qemu-system-arm \
    -M vexpress-a9 \
    -kernel u-boot \
    -nographic \
    -net nic \
    -net user,tftp=/srv/tftp
```

#### Configure U-Boot Network

```bash
# Set IP addresses
U-Boot> setenv ipaddr 192.168.1.100
U-Boot> setenv serverip 192.168.1.1
U-Boot> setenv netmask 255.255.255.0
U-Boot> setenv gatewayip 192.168.1.1

# Set MAC address (if not auto-assigned)
U-Boot> setenv ethaddr 52:54:00:12:34:56

# Save settings
U-Boot> saveenv
```

#### Test Network Connectivity

```bash
# Ping the server
U-Boot> ping 192.168.1.1
# Output: host 192.168.1.1 is alive

# If ping fails, check:
# 1. Cable/connection
# 2. IP addresses
# 3. Firewall on server
```

#### Test TFTP Download

```bash
# Download test file
U-Boot> tftp 0x60000000 test.txt
# Output:
# Using smc911x-0 device
# TFTP from server 192.168.1.1; our IP address is 192.168.1.100
# Filename 'test.txt'.
# Load address: 0x60000000
# Loading: #
# Bytes transferred = 15 (f hex)

# Verify content
U-Boot> md.b 0x60000000 10
# 60000000: 54 46 54 50 20 54 65 73 74 20    TFTP Test
```

---

### Part C: Load Kernel + DTB via TFTP

#### Step 1: Prepare Files on TFTP Server

```bash
# On your laptop
sudo cp /path/to/zImage /srv/tftp/
sudo cp /path/to/vexpress-v2p-ca9.dtb /srv/tftp/
sudo chmod 644 /srv/tftp/*
```

#### Step 2: Load Files via TFTP in U-Boot

```bash
# Load kernel
U-Boot> tftp 0x60008000 zImage
# Output: Bytes transferred = 4000000

# Load device tree
U-Boot> tftp 0x65000000 vexpress-v2p-ca9.dtb
# Output: Bytes transferred = 14692

# Optional: Load initramfs
U-Boot> tftp 0x66000000 initramfs.cpio.gz
```

#### Step 3: Set Boot Arguments and Boot

```bash
# Set kernel command line
U-Boot> setenv bootargs "console=ttyAMA0 root=/dev/nfs ip=dhcp nfsroot=192.168.1.1:/srv/nfs"

# Boot the kernel
U-Boot> bootz 0x60008000 - 0x65000000
```

#### Automated TFTP Boot Script

```bash
# Create boot script
U-Boot> setenv tftp_boot 'tftp 0x60008000 zImage; tftp 0x65000000 vexpress-v2p-ca9.dtb; bootz 0x60008000 - 0x65000000'

# Set as default boot command
U-Boot> setenv bootcmd 'run tftp_boot'

# Save
U-Boot> saveenv

# Now U-Boot will automatically boot via TFTP
```

### Complete TFTP Boot Flow

```
┌────────────────────────────────────────────────────────────────────┐
│                     TFTP BOOT SEQUENCE                              │
└────────────────────────────────────────────────────────────────────┘

U-Boot Starts
      │
      ▼
┌─────────────────┐
│ setenv ipaddr   │ Configure network
│ setenv serverip │
└────────┬────────┘
         │
         ▼
┌─────────────────┐     ┌─────────────────┐
│ tftp zImage     │────►│  TFTP Server    │
└────────┬────────┘     │  192.168.1.1    │
         │◄─────────────│  sends zImage   │
         │              └─────────────────┘
         ▼
┌─────────────────┐     ┌─────────────────┐
│ tftp board.dtb  │────►│  TFTP Server    │
└────────┬────────┘     │  sends DTB      │
         │◄─────────────└─────────────────┘
         │
         ▼
┌─────────────────┐
│ bootz kernel dtb│ Boot Linux kernel
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Linux Running   │
└─────────────────┘
```

---

## 8. Difference Between run and go Commands

### Question: What is the difference between run and go commands?

### Answer

| Aspect           | `run`                              | `go`                              |
| ---------------- | ---------------------------------- | --------------------------------- |
| **Purpose**      | Execute U-Boot environment script  | Jump to memory address            |
| **Input**        | Variable name                      | Memory address                    |
| **Executes**     | U-Boot commands (text)             | Binary code (machine instructions)|
| **Returns**      | Yes, to U-Boot prompt              | No, doesn't return (usually)      |
| **Use Case**     | Run saved boot scripts             | Start bare-metal program/kernel   |

### The `run` Command

Executes a sequence of U-Boot commands stored in an environment variable.

```bash
# Define a script in environment variable
U-Boot> setenv myboot 'echo Starting boot; fatload mmc 0:1 0x60008000 zImage; bootz 0x60008000'

# Execute the script
U-Boot> run myboot
Starting boot
4000000 bytes read in 250 ms
## Booting kernel...
```

**Characteristics:**
- Interprets text as U-Boot commands
- Multiple commands separated by semicolons
- Returns to U-Boot prompt if commands complete
- Can be nested (scripts calling other scripts)

### The `go` Command

Jumps to a memory address and starts executing machine code.

```bash
# Load a program to memory
U-Boot> fatload mmc 0:1 0x60000000 myprogram.bin

# Jump to and execute the program
U-Boot> go 0x60000000
## Starting application at 0x60000000 ...
```

**Characteristics:**
- Executes raw machine code at address
- Does NOT return to U-Boot (unless program explicitly returns)
- Used for bare-metal applications
- Similar to function call in C

### Visual Comparison

```
┌────────────────────────────────────────────────────────────────────┐
│                     RUN vs GO COMPARISON                            │
└────────────────────────────────────────────────────────────────────┘

┌─────────────── RUN COMMAND ──────────────────┐
│                                              │
│  Environment Variable: myboot                │
│  ┌────────────────────────────────────────┐  │
│  │ "echo hello; fatload mmc 0:1 ..."      │  │
│  └────────────────────────────────────────┘  │
│              │                               │
│              ▼                               │
│  ┌────────────────────────────────────────┐  │
│  │ U-Boot Command Parser                  │  │
│  │ Interprets as text commands            │  │
│  └────────────────────────────────────────┘  │
│              │                               │
│              ▼                               │
│         Executes each command                │
│              │                               │
│              ▼                               │
│         Returns to U-Boot prompt             │
└──────────────────────────────────────────────┘

┌─────────────── GO COMMAND ───────────────────┐
│                                              │
│  Memory Address: 0x60000000                  │
│  ┌────────────────────────────────────────┐  │
│  │ Binary machine code                    │  │
│  │ (ARM instructions)                     │  │
│  └────────────────────────────────────────┘  │
│              │                               │
│              ▼                               │
│  ┌────────────────────────────────────────┐  │
│  │ CPU Program Counter = 0x60000000       │  │
│  │ Start executing instructions           │  │
│  └────────────────────────────────────────┘  │
│              │                               │
│              ▼                               │
│         Program runs (no return)             │
│              │                               │
│              ▼                               │
│         U-Boot is "gone"                     │
└──────────────────────────────────────────────┘
```

### Practical Examples

```bash
# ============ RUN EXAMPLES ============

# Define and run simple script
U-Boot> setenv hello_script 'echo Hello World; echo Goodbye'
U-Boot> run hello_script
Hello World
Goodbye
U-Boot>    # Returns to prompt

# Chained scripts
U-Boot> setenv load_kernel 'fatload mmc 0:1 0x60008000 zImage'
U-Boot> setenv load_dtb 'fatload mmc 0:1 0x65000000 board.dtb'
U-Boot> setenv boot_linux 'run load_kernel; run load_dtb; bootz 0x60008000 - 0x65000000'
U-Boot> run boot_linux

# ============ GO EXAMPLES ============

# Run bare-metal "Hello World" program
U-Boot> fatload mmc 0:1 0x60000000 hello.bin
U-Boot> go 0x60000000
## Starting application at 0x60000000 ...
Hello from bare-metal!
# (might not return to U-Boot)

# Run with arguments
U-Boot> go 0x60000000 arg1 arg2
```

### When to Use Each

| Use `run` When...                    | Use `go` When...                      |
| ------------------------------------ | ------------------------------------- |
| Executing U-Boot commands            | Running bare-metal code               |
| Running boot scripts                 | Testing custom programs               |
| Need to return to U-Boot             | Launching standalone applications     |
| Combining multiple operations        | Jumping to specific code              |

---

## 9. Purpose of bootargs

### Question: What is the purpose of bootargs and who reads it?

### Answer

### What is bootargs?

`bootargs` is an environment variable that contains the **kernel command line** - parameters passed to the Linux kernel at boot time.

### Who Reads bootargs?

```
┌────────────────────────────────────────────────────────────────────┐
│                    BOOTARGS FLOW                                    │
└────────────────────────────────────────────────────────────────────┘

┌──────────────────┐
│    U-Boot        │
│                  │
│ bootargs =       │
│ "console=..."    │──────┐
└──────────────────┘      │
                          │ Passed via:
                          │ 1. Device Tree (chosen node)
                          │ 2. ATAGS (legacy)
                          │ 3. Direct memory
                          ▼
                   ┌──────────────────┐
                   │   Linux Kernel   │◄── READS bootargs
                   │                  │
                   │ Parses and uses  │
                   │ the parameters   │
                   └────────┬─────────┘
                            │
              ┌─────────────┼─────────────┐
              ▼             ▼             ▼
        ┌─────────┐   ┌─────────┐   ┌─────────┐
        │ Console │   │ Root FS │   │ Init    │
        │ Driver  │   │ Mount   │   │ Process │
        └─────────┘   └─────────┘   └─────────┘
```

### Common bootargs Parameters

| Parameter           | Purpose                          | Example                          |
| ------------------- | -------------------------------- | -------------------------------- |
| `console=`          | Kernel console output device     | `console=ttyS0,115200`           |
| `root=`             | Root filesystem device           | `root=/dev/mmcblk0p2`            |
| `rootwait`          | Wait for root device             | `rootwait`                       |
| `rootfstype=`       | Filesystem type                  | `rootfstype=ext4`                |
| `init=`             | First process to run             | `init=/sbin/init`                |
| `ip=`               | Network configuration            | `ip=dhcp`                        |
| `mem=`              | Memory limit                     | `mem=512M`                       |
| `quiet`             | Reduce boot messages             | `quiet`                          |
| `debug`             | Enable debug output              | `debug`                          |
| `rw` / `ro`         | Mount root read-write/read-only  | `rw`                             |

### Example bootargs

```bash
# Raspberry Pi with SD card root
setenv bootargs "console=ttyAMA0,115200 root=/dev/mmcblk0p2 rootwait rw"

# NFS root filesystem
setenv bootargs "console=ttyAMA0 root=/dev/nfs ip=dhcp nfsroot=192.168.1.1:/srv/nfs,vers=3"

# Minimal debug boot
setenv bootargs "console=ttyS0,115200 root=/dev/sda1 rootwait debug"

# QEMU Vexpress
setenv bootargs "console=ttyAMA0 root=/dev/mmcblk0p2 rootwait"
```

### How bootargs is Passed

#### Method 1: Via Device Tree (Modern)

U-Boot modifies the Device Tree's `/chosen` node:

```dts
/ {
    chosen {
        bootargs = "console=ttyAMA0 root=/dev/mmcblk0p2 rootwait";
    };
};
```

```bash
# U-Boot does this automatically when you run:
U-Boot> bootz 0x60008000 - 0x65000000
# bootargs is inserted into DTB at chosen/bootargs
```

#### Method 2: Via ATAGS (Legacy ARM)

Older method using memory-based tags at a fixed address.

### Viewing bootargs in Linux

```bash
# After Linux boots
cat /proc/cmdline
# Output: console=ttyAMA0 root=/dev/mmcblk0p2 rootwait rw

# Kernel messages show it too
dmesg | grep "Command line"
# Output: Kernel command line: console=ttyAMA0 root=/dev/mmcblk0p2 rootwait rw
```

### Summary Table

| Aspect           | Details                                    |
| ---------------- | ------------------------------------------ |
| **Set by**       | U-Boot (user configures)                   |
| **Read by**      | Linux Kernel                               |
| **Passed via**   | Device Tree `/chosen` node or ATAGS        |
| **Contains**     | Kernel command line parameters             |
| **Purpose**      | Configure kernel behavior at boot          |

---

## 10. Kernel Address Selection

### Question: Why do we use 0x62000000 and not 0x60000000 for kernel address on Raspberry Pi?

### Answer

### Short Answer

We cannot use 0x60000000 because:
1. **Lower addresses may be reserved** for exception vectors, ATAGs, or other boot structures
2. **U-Boot may be running** at or near the base address
3. **The kernel needs specific alignment** and safe memory regions
4. **Overlapping would corrupt** U-Boot or boot data before kernel takes over

### Memory Layout Analysis

```
┌────────────────────────────────────────────────────────────────────┐
│              RASPBERRY PI MEMORY MAP (Why not 0x60000000?)          │
└────────────────────────────────────────────────────────────────────┘

Note: Raspberry Pi DRAM actually starts at 0x00000000, not 0x60000000
(0x60000000 is for Vexpress-A9 in QEMU)

┌─────────────────────────────────────────────────────────────────┐
│ 0x00000000 ┌────────────────────────────────────────────────┐   │
│            │ ARM Exception Vectors (reserved)               │   │
│ 0x00000100 ├────────────────────────────────────────────────┤   │
│            │ ATAGS / Boot parameters                        │   │
│ 0x00008000 ├────────────────────────────────────────────────┤   │
│            │ Traditional Linux kernel load address          │   │
│            │ (32-bit ARM: 0x8000 offset from RAM start)     │   │
│            │                                                │   │
│ 0x00080000 ├────────────────────────────────────────────────┤   │
│            │ Alternative kernel location (RPi default)      │   │
│            │                                                │   │
│ 0x02000000 ├────────────────────────────────────────────────┤   │
│            │ Device Tree Blob location                      │   │
│            │                                                │   │
│ 0x02100000 ├────────────────────────────────────────────────┤   │
│            │ Initramfs location                             │   │
│            │                                                │   │
└────────────┴────────────────────────────────────────────────┴───┘
```

### For Vexpress-A9 (QEMU) Specifically

```
┌────────────────────────────────────────────────────────────────────┐
│              VEXPRESS-A9 MEMORY MAP                                 │
└────────────────────────────────────────────────────────────────────┘

DRAM starts at 0x60000000

0x60000000 ┌────────────────────────────────────────────────┐
           │ NOT RECOMMENDED FOR KERNEL!                    │
           │ • U-Boot may use this area                     │
           │ • Exception vectors may be here                │
           │ • Boot parameters (ATAGS) stored here          │
           │                                                │
0x60008000 ├────────────────────────────────────────────────┤
           │ Typical kernel load address                    │
           │ (0x8000 offset from RAM start)                 │
           │                                                │
           │ OR                                             │
           │                                                │
0x62000000 ├────────────────────────────────────────────────┤
           │ Safe kernel load address                       │
           │ • Avoids U-Boot workspace                      │
           │ • Avoids stack                                 │
           │ • Provides plenty of headroom                  │
           │                                                │
0x65000000 ├────────────────────────────────────────────────┤
           │ Device Tree Blob                               │
           │                                                │
0x66000000 ├────────────────────────────────────────────────┤
           │ Initramfs                                      │
           │                                                │
0x67000000 ├────────────────────────────────────────────────┤
           │ U-Boot relocated here (high memory)            │
           │                                                │
0x68000000 └────────────────────────────────────────────────┘
           END OF 128MB DRAM
```

# U-Boot Build and Deployment - README (Continued)

---

### Reasons for Address Selection (Continued)

| Issue                    | Why Not 0x60000000              | Why 0x62000000 Works          |
| ------------------------ | ------------------------------- | ----------------------------- |
| **Exception Vectors**    | ARM vectors at 0x0 or mirrored  | Far from vectors              |
| **U-Boot Code**          | U-Boot may be loaded here       | U-Boot relocates to high mem  |
| **Stack/Heap**           | U-Boot stack may overlap        | Clear of U-Boot workspace     |
| **ATAGS/DTB**            | Boot params at low offsets      | Separate from boot params     |
| **Kernel Requirements**  | Kernel expects specific offset  | Meets alignment requirements  |
| **Safety Margin**        | Risk of overwriting boot code   | Safe buffer zone              |

### Kernel Load Address Requirements

```
┌────────────────────────────────────────────────────────────────────┐
│              LINUX KERNEL LOAD REQUIREMENTS                         │
└────────────────────────────────────────────────────────────────────┘

For 32-bit ARM (zImage):
├── Must be loaded at TEXT_OFFSET from start of RAM
├── TEXT_OFFSET is typically 0x8000 (32KB)
├── So: DRAM_START + 0x8000
└── Example: 0x60000000 + 0x8000 = 0x60008000

For 64-bit ARM (Image):
├── Must be loaded at 2MB aligned address
├── TEXT_OFFSET is typically 0x80000 (512KB)
└── Example: 0x00080000 on RPi

Decompression Space:
├── zImage needs space ABOVE load address to decompress
├── If loaded too low, decompression may overwrite U-Boot
└── Higher address provides safety margin
```

### The Self-Decompression Problem

```
┌────────────────────────────────────────────────────────────────────┐
│              ZIMAGE DECOMPRESSION                                   │
└────────────────────────────────────────────────────────────────────┘

Initial State (kernel loaded at 0x60008000):
┌─────────────────────────────────────────────────────────────────┐
│ 0x60000000 │ Boot data/ATAGS                                    │
│ 0x60008000 │ ████████████████████████                           │
│            │ █ Compressed zImage    █ (4MB compressed)          │
│            │ ████████████████████████                           │
│ 0x60408000 │ (end of compressed image)                          │
│            │                                                    │
│ 0x67000000 │ U-Boot (relocated)                                 │
└─────────────────────────────────────────────────────────────────┘

During Decompression:
┌─────────────────────────────────────────────────────────────────┐
│ 0x60000000 │ Boot data                                          │
│ 0x60008000 │ ████████████████████████████████████████████████   │
│            │ █ Decompressed kernel image (12MB+)            █   │
│            │ ████████████████████████████████████████████████   │
│ 0x60C08000 │ (end of decompressed image)                        │
│            │                                                    │
│ 0x67000000 │ U-Boot (relocated) - MUST NOT BE OVERWRITTEN!      │
└─────────────────────────────────────────────────────────────────┘

If loaded at 0x60000000:
┌─────────────────────────────────────────────────────────────────┐
│ 0x60000000 │ ████████████████████████                           │
│            │ █ Compressed zImage    █                           │
│            │ ████████████████████████                           │
│            │       │                                            │
│            │       ▼ Decompresses DOWNWARD - OVERWRITES WHAT?   │
│            │                                                    │
│            │ ⚠️ PROBLEM: Nothing below to overwrite, but         │
│            │    violates TEXT_OFFSET requirement                │
└─────────────────────────────────────────────────────────────────┘
```

### Raspberry Pi Specific Addresses

For **Raspberry Pi 3/4** (64-bit):

```bash
# Standard addresses used by Raspberry Pi
kernel_addr=0x00080000    # 64-bit kernel (Image)
kernel_addr=0x00008000    # 32-bit kernel (zImage)
fdt_addr=0x02600000       # Device Tree
initrd_addr=0x02700000    # Initial ramdisk
```

For **Vexpress-A9** (QEMU, 32-bit):

```bash
# DRAM starts at 0x60000000
kernel_addr=0x60008000    # Standard: RAM + 0x8000
# OR
kernel_addr=0x62000000    # Safe: With larger margin

fdt_addr=0x65000000       # Device Tree
initrd_addr=0x66000000    # Initial ramdisk
```

### Why 0x62000000 Specifically?

```
┌────────────────────────────────────────────────────────────────────┐
│              CHOOSING 0x62000000                                    │
└────────────────────────────────────────────────────────────────────┘

0x62000000 = 0x60000000 + 0x2000000
           = DRAM_START + 32MB offset

Benefits of 32MB offset:
├── 1. Leaves 32MB for U-Boot workspace
├── 2. Plenty of room for boot parameters
├── 3. Allows for large initramfs below kernel
├── 4. Safe from any U-Boot memory usage
└── 5. Well-aligned (2MB boundary for 64-bit)

Memory layout with 0x62000000:
┌─────────────────────────────────────────────────────────────────┐
│ 0x60000000 - 0x60FFFFFF │ Boot workspace (16MB)                 │
│ 0x61000000 - 0x61FFFFFF │ Available/Initrd (16MB)               │
│ 0x62000000 - 0x64FFFFFF │ Kernel (48MB max)                     │
│ 0x65000000 - 0x65FFFFFF │ Device Tree (16MB)                    │
│ 0x66000000 - 0x67FFFFFF │ U-Boot relocated (32MB)               │
└─────────────────────────────────────────────────────────────────┘
```

### Practical Example

```bash
# In U-Boot, check where things can safely go:
U-Boot> bdinfo
DRAM bank   = 0x00000000
-> start    = 0x60000000
-> size     = 0x08000000    # 128MB

# Safe addresses for 128MB DRAM:
U-Boot> setenv kernel_addr_r 0x62000000
U-Boot> setenv fdt_addr_r 0x65000000
U-Boot> setenv ramdisk_addr_r 0x65100000

# Load and boot
U-Boot> tftp ${kernel_addr_r} zImage
U-Boot> tftp ${fdt_addr_r} vexpress-v2p-ca9.dtb
U-Boot> bootz ${kernel_addr_r} - ${fdt_addr_r}
```

### Address Selection Guidelines

| Memory Size | Kernel Address | DTB Address | Initrd Address |
| ----------- | -------------- | ----------- | -------------- |
| 128MB       | 0x60008000     | 0x65000000  | 0x66000000     |
| 256MB       | 0x62000000     | 0x68000000  | 0x69000000     |
| 512MB       | 0x62000000     | 0x6A000000  | 0x6C000000     |
| 1GB+        | 0x62000000     | 0x68000000  | 0x6A000000     |

### Summary

```
┌────────────────────────────────────────────────────────────────────┐
│                         SUMMARY                                     │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│  Q: Why not 0x60000000 for kernel?                                │
│                                                                    │
│  A: Multiple reasons:                                              │
│     1. ARM requires TEXT_OFFSET (typically 0x8000)                │
│     2. Boot parameters stored at low addresses                     │
│     3. U-Boot workspace may be in use                             │
│     4. Exception vectors need protection                           │
│     5. Kernel decompression needs space                            │
│                                                                    │
│  Q: Why 0x62000000?                                               │
│                                                                    │
│  A: Safe choice because:                                          │
│     1. 32MB offset from RAM start                                 │
│     2. Clears all U-Boot memory usage                             │
│     3. Well-aligned for ARM requirements                          │
│     4. Leaves room for DTB and initrd above                       │
│     5. Standard convention for this platform                       │
│                                                                    │
└────────────────────────────────────────────────────────────────────┘
```

---

