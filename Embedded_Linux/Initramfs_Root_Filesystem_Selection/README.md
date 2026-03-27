# Initramfs-Based Root Filesystem Selection

A minimal initramfs implementation that allows users to select between SD card and NFS root filesystem during boot on Raspberry Pi 3.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Directory Structure](#directory-structure)
- [Build Instructions](#build-instructions)
  - [1. Create Initramfs Directory](#1-create-initramfs-directory)
  - [2. Build Static BusyBox](#2-build-static-busybox)
  - [3. Install BusyBox Binaries](#3-install-busybox-binaries)
  - [4. Create Init Script](#4-create-init-script)
  - [5. Create Boot Selection Script (rcS)](#5-create-boot-selection-script-rcs)
  - [6. Generate Initramfs Image](#6-generate-initramfs-image)
- [Deployment](#deployment)
- [Usage](#usage)
- [Configuration](#configuration)
- [Troubleshooting](#troubleshooting)

---

## Overview

This project implements a boot selection mechanism using initramfs. The initramfs acts as an intermediate environment that:

1. Initializes the system with minimal components
2. Presents a menu for the user to choose the root filesystem source
3. Mounts the selected root filesystem
4. Switches to the new root and continues the boot process

---

## Features

- **Dual Boot Support**: Choose between SD card or NFS root filesystem
- **Minimal Footprint**: Uses statically linked BusyBox for small image size
- **No Library Dependencies**: Static linking eliminates runtime library issues
- **Network Boot Ready**: Supports NFS root with configurable network settings
- **Error Recovery**: Falls back to shell on errors for debugging

---

## Prerequisites

- Cross-compiler toolchain for ARM64 (aarch64-rpi3-linux-gnu)
- BusyBox source code
- U-Boot with mkimage tool
- Target: Raspberry Pi 3 Model B/B+

---

## Directory Structure

```
rootfs_initramfs_Slink/
├── bin/
│   ├── busybox          # Main static binary
│   ├── sh -> busybox
│   ├── mount -> busybox
│   ├── umount -> busybox
│   ├── mkdir -> busybox
│   ├── echo -> busybox
│   ├── sleep -> busybox
│   ├── chroot -> busybox
│   └── cat -> busybox
├── sbin/
│   ├── init             # Init script (calls rcS)
│   ├── mdev -> ../bin/busybox
│   ├── ifconfig -> ../bin/busybox
│   └── route -> ../bin/busybox
├── etc/
│   └── init.d/
│       └── rcS          # Boot selection script
├── dev/
├── proc/
├── sys/
└── newroot/
```

---

## Build Instructions

### 1. Create Initramfs Directory

Create the root directory structure for the initramfs:

```bash
mkdir rootfs_initramfs_Slink
```

Create the necessary subdirectories:

```bash
mkdir -p rootfs_initramfs_Slink/bin
mkdir -p rootfs_initramfs_Slink/sbin
```

---

### 2. Build Static BusyBox

Navigate to the BusyBox source directory and configure the build environment:

```bash
cd busybox_repo_Slink
export CROSS_COMPILE=~/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-
export ARCH=arm64
make distclean
make defconfig
```

Configure BusyBox for static linking:

```bash
make menuconfig
```

**In menuconfig, apply the following settings:**

1. Navigate to `Settings` → `Build Options`
2. Enable `[*] Build static binary (no shared libs)`
3. Navigate to `Settings` → `Library Tuning`
4. Disable `[ ] SHA1: Use hardware accelerated instructions if possible`
5. Disable `[ ] SHA256: Use hardware accelerated instructions if possible`

> **Note:** Disabling SHA hardware acceleration is required for ARM64 cross-compilation compatibility.

Build and install BusyBox:

```bash
make -j$(nproc)
make CONFIG_PREFIX=./static_install install
```

---

### 3. Install BusyBox Binaries

Copy the static BusyBox binary to the initramfs:

```bash
cp -a busybox_repo_Slink/static_install/bin/busybox rootfs_initramfs_Slink/bin/
```

Create symlinks for required commands in `/bin`:

```bash
cd rootfs_initramfs_Slink/bin
ln -sf busybox sh
ln -sf busybox mount
ln -sf busybox umount
ln -sf busybox mkdir
ln -sf busybox echo
ln -sf busybox sleep
ln -sf busybox chroot
ln -sf busybox cat
```

Create symlinks for required commands in `/sbin`:

```bash
cd ../sbin
ln -sf ../bin/busybox mdev
ln -sf ../bin/busybox ifconfig
ln -sf ../bin/busybox route
```

---

### 4. Create Init Script

Create the init script that executes rcS at boot:

```bash
cat << 'EOF' > rootfs_initramfs_Slink/sbin/init
#!/bin/sh
exec /etc/init.d/rcS
EOF
chmod +x rootfs_initramfs_Slink/sbin/init
```

---

### 5. Create Boot Selection Script (rcS)

Create the directory structure and the main boot selection script:

```bash
mkdir -p rootfs_initramfs_Slink/etc/init.d
cat << 'EOF' > rootfs_initramfs_Slink/etc/init.d/rcS
#!/bin/sh
# =============================================================================
# Initramfs Boot Selection Script
#
# This script runs as PID 1 from /sbin/init in the initramfs.
# It allows the user to choose between booting from:
#   1) SD card root filesystem
#   2) NFS root filesystem
#
# After mounting the selected root filesystem, it switches to the new root
# using chroot and executes the real /sbin/init.
# =============================================================================

# -----------------------------------------------------------------------------
# Set PATH for BusyBox commands
# -----------------------------------------------------------------------------
PATH=/bin:/sbin
export PATH

# -----------------------------------------------------------------------------
# Configuration Variables
# Modify these values according to your network and storage setup
# -----------------------------------------------------------------------------

# Network interface name (usually eth0 for wired Ethernet)
NET_IF="eth0"

# Static IP address assigned to this board
CLIENT_IP="192.168.1.100"

# Default gateway (router) IP address
GATEWAY="192.168.1.1"

# NFS server IP address
NFS_SERVER="192.168.1.50"

# NFS exported root filesystem path on the server
NFS_PATH="/media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/sd_card/rootfs"

# SD card root filesystem partition device
SD_ROOT="/dev/mmcblk0p2"

# Temporary mount point for the selected root filesystem
NEWROOT="/newroot"

# -----------------------------------------------------------------------------
# Initial Setup
# Create required directories and mount virtual filesystems
# -----------------------------------------------------------------------------

# Create necessary directories
mkdir -p /proc /sys /dev "$NEWROOT"

# Mount proc filesystem (provides process and kernel information)
mount -t proc proc /proc

# Mount sysfs filesystem (provides kernel and device information)
mount -t sysfs sysfs /sys

# Mount devtmpfs filesystem (provides device nodes automatically)
mount -t devtmpfs devtmpfs /dev

# Populate device nodes using mdev (BusyBox device manager)
mdev -s

# -----------------------------------------------------------------------------
# Boot Menu
# Display options and read user selection
# -----------------------------------------------------------------------------


# Wait a moment for the system to stabilize before showing the menu
sleep 5

echo "========================================"
echo "       Initramfs Boot Selector"
echo "========================================"
echo ""
echo "Choose boot source:"
echo "  1) SD card"
echo "  2) NFS"
echo ""
echo -n "Enter choice [1-2]: "
read choice

# -----------------------------------------------------------------------------
# Option 1: Boot from SD Card
# -----------------------------------------------------------------------------

if [ "$choice" = "1" ]; then
    echo ""
    echo "Booting from SD card..."

    # Wait for SD card to be ready
    sleep 2

    # Mount the SD card root filesystem partition
    mount "$SD_ROOT" "$NEWROOT" || {
        echo "ERROR: Failed to mount SD card rootfs from $SD_ROOT"
        exec sh
    }

# -----------------------------------------------------------------------------
# Option 2: Boot from NFS
# -----------------------------------------------------------------------------

elif [ "$choice" = "2" ]; then
    echo ""
    echo "Booting from NFS..."

    # Configure network interface with static IP address
    ifconfig "$NET_IF" "$CLIENT_IP" up

    # Add default gateway for network routing
    route add default gw "$GATEWAY"

    # Wait for network link to become ready
    sleep 5

    # Mount the NFS exported root filesystem
    # Options:
    #   nolock  - disable NFS file locking (required for some setups)
    #   vers=3  - use NFS version 3
    #   tcp     - use TCP protocol for reliability
    mount -t nfs -o nolock,vers=3,tcp "${NFS_SERVER}:${NFS_PATH}" "$NEWROOT" || {
        echo "ERROR: Failed to mount NFS rootfs from ${NFS_SERVER}:${NFS_PATH}"
        exec sh
    }

# -----------------------------------------------------------------------------
# Invalid Choice
# -----------------------------------------------------------------------------

else
    echo ""
    echo "ERROR: Invalid choice"
    exec sh
fi

# -----------------------------------------------------------------------------
# Verify Root Filesystem
# Check that the selected root filesystem contains /sbin/init
# -----------------------------------------------------------------------------

if [ ! -x "$NEWROOT/sbin/init" ]; then
    echo "ERROR: /sbin/init not found in selected root filesystem"
    exec sh
fi

# -----------------------------------------------------------------------------
# Prepare New Root Filesystem
# Mount virtual filesystems inside the new root before switching
# -----------------------------------------------------------------------------

# Create mount points if they don't exist
mkdir -p "$NEWROOT/dev" "$NEWROOT/proc" "$NEWROOT/sys"

# Mount devtmpfs in new root (provides /dev/console and other device nodes)
mount -t devtmpfs devtmpfs "$NEWROOT/dev" 2>/dev/null

# Mount proc in new root (required by many applications)
mount -t proc proc "$NEWROOT/proc" 2>/dev/null

# Mount sysfs in new root (required by many applications)
mount -t sysfs sysfs "$NEWROOT/sys" 2>/dev/null

# -----------------------------------------------------------------------------
# Switch to New Root Filesystem
# Use chroot to change root directory and execute the real init
# -----------------------------------------------------------------------------

echo ""
echo "Switching to new root filesystem..."
exec chroot "$NEWROOT" /sbin/init
EOF
```

Make the rcS script executable:

```bash
chmod +x rootfs_initramfs_Slink/etc/init.d/rcS
```

---

### 6. Generate Initramfs Image

Create the compressed cpio archive:

```bash
cd rootfs_initramfs_Slink
find . | cpio -H newc -ov 2>/dev/null | gzip > ../initramfs.cpio.gz
cd -
```

Create U-Boot compatible image:

```bash
mkimage -A arm64 -T ramdisk -C gzip \
    -n "Initramfs rootfs selection" \
    -d initramfs.cpio.gz uInitramfs
```

---

## Deployment

Copy the initramfs image to TFTP server:

```bash
sudo cp uInitramfs /srv/tftp/
```

Copy to SD card boot partition:

```bash
cp uInitramfs <path_to_sd_card>/bootfs
```

---

## Usage

1. Boot the Raspberry Pi with the initramfs
2. Wait for the boot menu to appear
3. Enter your choice:
   - `1` for SD card boot
   - `2` for NFS boot
4. The system will mount the selected root filesystem and continue booting

**Example Boot Output:**

```
========================================
       Initramfs Boot Selector
========================================

Choose boot source:
  1) SD card
  2) NFS

Enter choice [1-2]: 1

Booting from SD card...
Switching to new root filesystem...
System booted successfully!
```

---

## Configuration

Edit the configuration variables in `/etc/init.d/rcS` to match your setup:

| Variable | Description | Default |
|----------|-------------|---------|
| `NET_IF` | Network interface name | `eth0` |
| `CLIENT_IP` | Board IP address | `192.168.1.100` |
| `GATEWAY` | Default gateway | `192.168.1.1` |
| `NFS_SERVER` | NFS server IP | `192.168.1.50` |
| `NFS_PATH` | NFS export path | (your path) |
| `SD_ROOT` | SD card root partition | `/dev/mmcblk0p2` |

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| `chroot: not found` | Ensure chroot symlink exists in `/bin` |
| `mount: mounting NFS failed` | Check network configuration and NFS server exports |
| `/sbin/init not found` | Verify root filesystem contains BusyBox with init |
| `Device or resource busy` | Harmless warning, filesystems already mounted |
| Kernel panic after switch | Ensure rootfs BusyBox is static ARM64 |

---

## License

This project is provided as-is for educational purposes.

---
