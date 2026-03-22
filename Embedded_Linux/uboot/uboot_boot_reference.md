# U-Boot Kernel Loading Guide

## Overview

This repository explains how to load and boot a Linux kernel using U-Boot bootloader. When U-Boot starts, it automatically executes the `bootcmd` environment variable, which contains the boot sequence commands.

---

## Table of Contents

- [Loading Methods](#loading-methods)
  - [Loading from FAT Partition](#loading-from-fat-partition)
  - [Loading via TFTP](#loading-via-tftp)
- [Booting Methods](#booting-methods)
  - [Method 1: Manual bootcmd Script](#method-1-manual-bootcmd-script)
  - [Method 2: Compiled Boot Script (mkimage)](#method-2-compiled-boot-script-mkimage)
  - [Method 3: extlinux.conf with bootflow](#method-3-extlinuxconf-with-bootflow)

---

## Loading Methods

### Loading from FAT Partition

Load kernel and device tree from a FAT filesystem on an SD card or eMMC.

#### Prerequisites
- FAT formatted partition on storage device
- Kernel image (e.g., `zImage`, `Image`)
- Device tree blob (e.g., `board.dtb`)

#### Commands

```bash
# Set load addresses
setenv kernel_addr_r 0x80000000
setenv fdt_addr_r 0x83000000

# Load kernel from FAT partition
# fatload <interface> <dev[:part]> <addr> <filename>
fatload mmc 0:1 ${kernel_addr_r} zImage

# Load device tree
fatload mmc 0:1 ${fdt_addr_r} board.dtb

# (Optional) Load ramdisk
fatload mmc 0:1 ${ramdisk_addr_r} initramfs.img
```

#### Interface Options
| Interface | Description |
|-----------|-------------|
| `mmc 0:1` | First MMC device, first partition |
| `mmc 1:1` | Second MMC device, first partition |
| `usb 0:1` | First USB device, first partition |

---

### Loading via TFTP

Load kernel and device tree from a TFTP server over the network.

#### Prerequisites
- Network connection configured
- TFTP server running on host machine
- Kernel and DTB files in TFTP server directory

#### Network Configuration

```bash
# Set static IP
setenv ipaddr 192.168.1.100
setenv serverip 192.168.1.1
setenv netmask 255.255.255.0
setenv gatewayip 192.168.1.1

# Or use DHCP
dhcp
```

#### Commands

```bash
# Set load addresses
setenv kernel_addr_r 0x80000000
setenv fdt_addr_r 0x83000000

# Load kernel via TFTP
# tftp <addr> <filename>
tftp ${kernel_addr_r} zImage

# Load device tree
tftp ${fdt_addr_r} board.dtb

# (Optional) Load ramdisk
tftp ${ramdisk_addr_r} initramfs.img
```

---

## Booting Methods

### Method 1: Manual bootcmd Script

Write boot commands directly to the `bootcmd` environment variable.

#### For FAT Boot

```bash
setenv bootargs 'console=ttyS0,115200 root=/dev/mmcblk0p2 rootwait rw'

setenv bootcmd 'fatload mmc 0:1 ${kernel_addr_r} zImage; \
                fatload mmc 0:1 ${fdt_addr_r} board.dtb; \
                bootz ${kernel_addr_r} - ${fdt_addr_r}'

# Save environment
saveenv

# Run boot command
run bootcmd
```

#### For TFTP Boot

```bash
setenv bootargs 'console=ttyS0,115200 root=/dev/nfs nfsroot=192.168.1.1:/nfsroot ip=dhcp'

setenv bootcmd 'tftp ${kernel_addr_r} zImage; \
                tftp ${fdt_addr_r} board.dtb; \
                bootz ${kernel_addr_r} - ${fdt_addr_r}'

# Save environment
saveenv

# Run boot command
run bootcmd
```

#### Boot Commands Reference
| Command | Architecture | Image Type |
|---------|--------------|------------|
| `bootz` | ARM 32-bit | zImage |
| `booti` | ARM 64-bit | Image |
| `bootm` | Any | uImage (legacy) |

---

### Method 2: Compiled Boot Script (mkimage)

Create a boot script file, compile it using `mkimage`, and execute it in U-Boot.

#### Step 1: Create Script File

Create `boot.script`:

```bash
# boot.script - Boot script for U-Boot

echo "Loading kernel..."
fatload mmc 0:1 ${kernel_addr_r} zImage

echo "Loading device tree..."
fatload mmc 0:1 ${fdt_addr_r} board.dtb

echo "Setting boot arguments..."
setenv bootargs 'console=ttyS0,115200 root=/dev/mmcblk0p2 rootwait rw'

echo "Booting kernel..."
bootz ${kernel_addr_r} - ${fdt_addr_r}
```

#### Step 2: Compile Script

```bash
# Install u-boot-tools if not available
sudo apt-get install u-boot-tools

# Compile the script
mkimage -C none -A arm -T script -d boot.script boot.scr
```

#### mkimage Options
| Option | Description |
|--------|-------------|
| `-C none` | No compression |
| `-A arm` | Architecture (arm, arm64, x86, etc.) |
| `-T script` | Image type is script |
| `-d` | Input file |

#### Step 3: Deploy Script

Copy `boot.scr` to the boot partition:

```bash
# Mount boot partition
sudo mount /dev/sdX1 /mnt

# Copy script
sudo cp boot.scr /mnt/

# Unmount
sudo umount /mnt
```

#### Step 4: Execute in U-Boot

```bash
# Load and execute script
fatload mmc 0:1 ${loadaddr} boot.scr
source ${loadaddr}

# Or set as bootcmd
setenv bootcmd 'fatload mmc 0:1 ${loadaddr} boot.scr; source ${loadaddr}'
saveenv
```

---

### Method 3: extlinux.conf with bootflow

Use the `extlinux.conf` configuration file with U-Boot's `bootflow` command for standardized boot configuration.

#### Step 1: Create Directory Structure

```bash
mkdir -p /boot/extlinux
```

#### Step 2: Create extlinux.conf

Create `/boot/extlinux/extlinux.conf`:

```ini
# extlinux.conf - Boot configuration

DEFAULT linux
TIMEOUT 30
PROMPT 1

MENU TITLE Boot Menu

LABEL linux
    MENU LABEL Linux Kernel
    LINUX /zImage
    FDT /board.dtb
    APPEND console=ttyS0,115200 root=/dev/mmcblk0p2 rootwait rw

LABEL linux-recovery
    MENU LABEL Linux Recovery Mode
    LINUX /zImage
    FDT /board.dtb
    APPEND console=ttyS0,115200 root=/dev/mmcblk0p2 rootwait rw single

LABEL linux-tftp
    MENU LABEL Linux via TFTP
    LINUX tftp://192.168.1.1/zImage
    FDT tftp://192.168.1.1/board.dtb
    APPEND console=ttyS0,115200 root=/dev/nfs nfsroot=192.168.1.1:/nfsroot ip=dhcp
```

#### Configuration Options

| Directive | Description |
|-----------|-------------|
| `DEFAULT` | Default boot entry label |
| `TIMEOUT` | Auto-boot timeout (in 1/10 seconds) |
| `PROMPT` | Show prompt (1) or not (0) |
| `LABEL` | Entry identifier |
| `MENU LABEL` | Display name in menu |
| `LINUX` | Path to kernel image |
| `FDT` | Path to device tree |
| `INITRD` | Path to initramfs (optional) |
| `APPEND` | Kernel command line arguments |

#### Step 3: Deploy Files

```bash
# Copy files to boot partition
sudo cp zImage /boot/
sudo cp board.dtb /boot/
sudo cp extlinux.conf /boot/extlinux/
```

#### Step 4: Boot Using bootflow

In U-Boot:

```bash
# Scan for bootable devices
bootflow scan

# List available bootflows
bootflow list

# Select and boot
bootflow select 0
bootflow boot

# Or simply
run bootcmd
```

#### Alternative: Set bootcmd for extlinux

```bash
setenv bootcmd 'bootflow scan; bootflow select 0; bootflow boot'
saveenv
```

---

## Quick Reference

### Environment Variables

```bash
# Common addresses (example values - check your board)
setenv kernel_addr_r 0x80000000
setenv fdt_addr_r 0x83000000
setenv ramdisk_addr_r 0x88000000
setenv loadaddr 0x80000000
```

### File Locations Summary

| Method | Location |
|--------|----------|
| Manual/FAT | Files on FAT partition root |
| Manual/TFTP | Files on TFTP server |
| Compiled Script | `boot.scr` on boot partition |
| extlinux | `/extlinux/extlinux.conf` on boot partition |

---

## Troubleshooting

### Common Issues

1. **"File not found"**: Check filename and partition number
2. **"Bad Magic Number"**: Ensure correct image format for boot command
3. **"FDT Error"**: Device tree address or format issue
4. **Network timeout**: Check IP configuration and server availability

### Useful U-Boot Commands

```bash
# Print environment
printenv

# Check MMC devices
mmc list
mmc dev 0
mmc part

# Check network
ping ${serverip}

# Memory info
bdinfo
```

---

## License

This documentation is provided under the MIT License.

---

## Contributing

Feel free to submit issues and pull requests to improve this documentation.
