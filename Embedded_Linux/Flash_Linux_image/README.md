# 🚀 Raspberry Pi 3 B+ Embedded Linux from Scratch

A complete guide to building and deploying a custom embedded Linux system on Raspberry Pi 3 Model B+ — including U-Boot, Linux kernel, BusyBox rootfs, and multiple boot methods (SD card, TFTP, NFS, initramfs).

---

## 📋 Table of Contents

- [Prerequisites](#prerequisites)
- [Project Structure](#project-structure)
- [1. Repository Setup](#1-repository-setup)
- [2. Build the Linux Kernel](#2-build-the-linux-kernel)
- [3. Build BusyBox Root Filesystem](#3-build-busybox-root-filesystem)
- [4. Prepare Boot Files](#4-prepare-boot-files)
- [5. Boot Methods](#5-boot-methods)
  - [5.1 SD Card Boot](#51-sd-card-boot)
  - [5.2 TFTP Boot](#52-tftp-boot)
  - [5.3 NFS Root Filesystem](#53-nfs-root-filesystem)
  - [5.4 Initramfs Boot](#54-initramfs-boot)
- [6. Flash to SD Card](#6-flash-to-sd-card)
- [Serial Console Configuration](#serial-console-configuration)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

| Tool | Purpose |
|------|---------|
| `aarch64-rpi3-linux-gnu-` cross-compiler | Cross-compilation toolchain (crosstool-NG) |
| `mkimage` | U-Boot image creation tool |
| `minicom` / `picocom` | Serial console terminal |
| `tftpd-hpa` | TFTP server for network boot |
| `nfs-kernel-server` | NFS server for network root filesystem |
| USB-to-Serial adapter | UART connection to RPi3 |

---

## Project Structure

```
Flash_Linux_image/
├── boot.cmd                    # U-Boot boot script source
├── boot.scr                    # Compiled U-Boot boot script
├── initramfs.cpio.gz           # Compressed initramfs archive
├── uInitramfs                  # U-Boot wrapped initramfs
├── linux_repo_Slink    → ...   # Symlink to Linux kernel source
├── uboot_repo_Slink    → ...   # Symlink to U-Boot source
├── busybox_repo_Slink  → ...   # Symlink to BusyBox source
└── sd_card/
    ├── bootfs/                 # Boot partition (FAT32)
    │   ├── bootcode.bin        # RPi first-stage bootloader
    │   ├── start.elf           # RPi second-stage bootloader
    │   ├── fixup.dat           # RPi firmware fixup
    │   ├── config.txt          # RPi configuration
    │   ├── cmdline.txt         # Kernel command line (unused with U-Boot)
    │   ├── u-boot.bin          # U-Boot bootloader
    │   ├── boot.scr            # U-Boot boot script
    │   ├── Image               # Linux kernel image (aarch64)
    │   ├── bcm2710-rpi-3-b-plus.dtb  # Device tree blob
    │   └── uInitramfs          # Initramfs image (optional)
    └── rootfs/                 # Root partition (ext4)
        ├── bin/                # BusyBox binaries
        ├── sbin/               # System binaries (init → busybox)
        ├── lib/                # Shared libraries (from toolchain sysroot)
        ├── lib64 → lib         # lib64 symlink
        ├── usr/                # User binaries and libraries
        ├── etc/                # Configuration files
        │   ├── inittab         # BusyBox init configuration
        │   └── init.d/
        │       └── rcS         # System startup script
        ├── dev/                # Device nodes
        ├── proc/               # Proc filesystem mount point
        ├── sys/                # Sysfs mount point
        ├── tmp/                # Temporary files
        └── home/               # Home directories
```

---

## 1. Repository Setup

### Create Symbolic Links

Link to the source repositories to keep the project organized:

```bash
ln -s /path/to/u-boot              uboot_repo_Slink
ln -s /path/to/linux                linux_repo_Slink
ln -s /path/to/busybox              busybox_repo_Slink
```

### Create SD Card Directory Structure

```bash
mkdir -p sd_card/bootfs
mkdir -p sd_card/rootfs
```

---

## 2. Build the Linux Kernel

### Clone the Raspberry Pi Kernel

```bash
git clone --depth 1 https://github.com/raspberrypi/linux.git
cd linux
```

### Configure and Build

```bash
# Configure for RPi3 B+
make ARCH=arm64 CROSS_COMPILE=aarch64-rpi3-linux-gnu- bcm2710_defconfig

# Build kernel image and device tree blobs
make ARCH=arm64 CROSS_COMPILE=aarch64-rpi3-linux-gnu- -j$(nproc) Image dtbs

# Install kernel modules to rootfs
make ARCH=arm64 CROSS_COMPILE=aarch64-rpi3-linux-gnu- \
    modules_install INSTALL_MOD_PATH=../sd_card/rootfs/
```

### Copy Build Artifacts

```bash
# To boot partition
cp arch/arm64/boot/Image                                    ../sd_card/bootfs/
cp arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dtb   ../sd_card/bootfs/

# To TFTP server (for network boot)
sudo cp arch/arm64/boot/Image                                   /srv/tftp/
sudo cp arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dtb  /srv/tftp/
```

---

## 3. Build BusyBox Root Filesystem

### Copy BusyBox Binaries

```bash
cp -r busybox_repo_Slink/_install/bin/  sd_card/rootfs/
cp -r busybox_repo_Slink/_install/sbin/ sd_card/rootfs/
cp -r busybox_repo_Slink/_install/usr/  sd_card/rootfs/
```

### Create Required Directories

```bash
mkdir -p sd_card/rootfs/{dev,proc,sys,etc,home,tmp}
```

### Copy Shared Libraries from Toolchain Sysroot

```bash
SYSROOT=~/x-tools/aarch64-rpi3-linux-gnu/aarch64-rpi3-linux-gnu/sysroot

cp -r ${SYSROOT}/lib       sd_card/rootfs/
sudo cp -r ${SYSROOT}/usr/lib  sd_card/rootfs/usr/

# Create lib64 symlink (required for aarch64)
cd sd_card/rootfs
ln -sf lib lib64
cd -
```

### Set Permissions

```bash
sudo chown -R root:root sd_card/rootfs/
sudo chmod -R 755 sd_card/rootfs/
```

### Create Init Configuration

**`sd_card/rootfs/etc/inittab`**:

```bash
# Start system initialization script
::sysinit:/etc/init.d/rcS

# Launch interactive shell on serial console
# askfirst: displays "Please press Enter to activate this console"
ttyS0::askfirst:-/bin/sh

# Handle Ctrl+Alt+Del
::ctrlaltdel:/sbin/reboot

# Clean shutdown
::shutdown:/bin/umount -a -r
```

### Create Startup Script

```bash
mkdir -p sd_card/rootfs/etc/init.d
```

**`sd_card/rootfs/etc/init.d/rcS`**:

```bash
#!/bin/sh

# Mount devtmpfs (required for initramfs; already mounted for SD/NFS boot)
mount -t devtmpfs devtmpfs /dev
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t tmpfs tmpfs /tmp

echo "System booted successfully!"
```

```bash
chmod +x sd_card/rootfs/etc/init.d/rcS
```

---

## 4. Prepare Boot Files

### Copy Raspberry Pi Firmware

```bash
# bootcode.bin, start.elf, fixup.dat, config.txt
sudo cp /path/to/rpi_boot_files/* sd_card/bootfs/
```

### Copy U-Boot Binary

```bash
sudo cp uboot_repo_Slink/u-boot.bin sd_card/bootfs/
```

### Create and Compile Boot Script

Write your boot script in `boot.cmd`, then compile:

```bash
mkimage -A arm64 -T script -C none -n "Boot Script" -d boot.cmd boot.scr
sudo cp boot.scr sd_card/bootfs/
```

---

## 5. Boot Methods

### 5.1 SD Card Boot

Boot entirely from the SD card — kernel, DTB, and rootfs all on the card.

**U-Boot bootargs:**

```
setenv bootargs "console=ttyS0,115200 8250.nr_uarts=1 root=/dev/mmcblk0p2 rootfstype=ext4 rw rootwait init=/sbin/init"
```

---

### 5.2 TFTP Boot

Load kernel and DTB from a TFTP server over the network.

#### Host Setup

```bash
# Install TFTP server
sudo apt install tftpd-hpa

# Configure /etc/default/tftpd-hpa
TFTP_USERNAME="tftp"
TFTP_DIRECTORY="/srv/tftp"
TFTP_ADDRESS=":69"
TFTP_OPTIONS="--secure --create"

# Start the server
sudo systemctl restart tftpd-hpa

# Copy files to TFTP directory
sudo cp Image bcm2710-rpi-3-b-plus.dtb /srv/tftp/

# Configure network interface (direct cable to Pi)
sudo nmcli device set eno1 managed no
sudo ip addr flush dev eno1
sudo ip addr add 192.168.1.50/24 dev eno1
sudo ip link set eno1 up
```

#### Network Configuration

```
Host PC (TFTP Server):  192.168.1.50
Raspberry Pi:           192.168.1.100
Gateway:                192.168.1.1
```

---

### 5.3 NFS Root Filesystem

Mount the root filesystem from an NFS server over the network — ideal for development (edit files on host, instantly available on target).

#### Host Setup

```bash
# Install NFS server
sudo apt install nfs-kernel-server

# Add export to /etc/exports
echo '/path/to/sd_card/rootfs 192.168.1.100(rw,sync,no_subtree_check,no_root_squash)' | \
    sudo tee -a /etc/exports

# Apply and verify
sudo exportfs -ra
sudo exportfs -v
```

#### U-Boot bootargs

```
setenv bootargs "console=ttyS0,115200 8250.nr_uarts=1 root=/dev/nfs rw nfsroot=192.168.1.50:/path/to/sd_card/rootfs,v3,tcp ip=192.168.1.100:192.168.1.50:192.168.1.1:255.255.255.0::eth0:off init=/sbin/init"
```

---

### 5.4 Initramfs Boot

Package the entire rootfs into a compressed cpio archive loaded into RAM — fully self-contained, no storage needed after boot.

#### Build Initramfs

```bash
# IMPORTANT: must run from INSIDE the rootfs directory
cd sd_card/rootfs
find . | cpio -H newc -ov 2>/dev/null | gzip > ../../initramfs.cpio.gz
cd -

# Wrap with U-Boot header
mkimage -A arm64 -T ramdisk -C gzip \
    -n "Initramfs Image" \
    -d initramfs.cpio.gz uInitramfs

# Copy to boot partition and/or TFTP
cp uInitramfs sd_card/bootfs/
sudo cp uInitramfs /srv/tftp/
```

> ⚠️ **Common mistake:** Running `find` from outside the rootfs creates wrong paths (e.g., `sd_card/rootfs/bin/` instead of `./bin/`). Always `cd` into the rootfs first!

#### U-Boot bootargs

```
setenv bootargs "console=ttyS0,115200 8250.nr_uarts=1 loglevel=8 panic=5 rdinit=/sbin/init"
```

> Note: No `root=` parameter needed — the kernel uses the initramfs as the root filesystem.

---

## 6. Flash to SD Card

### Partition the SD Card

The SD card needs two partitions:

| Partition | Type | Size | Mount |
|-----------|------|------|-------|
| p1 | FAT32 (W95) | ~128 MB | `/boot` |
| p2 | ext4 (Linux) | Remaining | `/` |

### Copy Files to SD Card

```bash
# Insert SD card and check device name
lsblk

# Copy boot files
sudo rsync -aHAX --progress sd_card/bootfs/ /media/$USER/<boot_partition>/

# Copy root filesystem
sudo rsync -aHAX --progress sd_card/rootfs/ /media/$USER/<root_partition>/

# Safely eject
sudo umount /media/$USER/<boot_partition>
sudo umount /media/$USER/<root_partition>
```

---

## Serial Console Configuration

### RPi3 B+ UART Setup

The RPi3 B+ has two UARTs. By default, the full PL011 UART is assigned to Bluetooth. To use the mini UART for console:

**`config.txt`:**

```
enable_uart=1
dtoverlay=miniuart-bt
```

| UART | Device | Usage |
|------|--------|-------|
| Mini UART (8250) | `ttyS0` | Console (GPIO 14/15) |
| PL011 | `ttyAMA1` | Bluetooth (swapped) |

### Wiring

```
USB-Serial Adapter         RPi3 GPIO Header
──────────────────         ─────────────────
TX  ──────────────────────  RX  (Pin 10 / GPIO 15)
RX  ──────────────────────  TX  (Pin 8  / GPIO 14)
GND ──────────────────────  GND (Pin 6)
```

> ⚠️ Do NOT connect VCC if the Pi is powered separately.

### Connect via Serial

```bash
# Using picocom (recommended)
sudo picocom -b 115200 /dev/ttyUSB0 --flow none

# Using minicom (disable hardware flow control!)
sudo minicom -b 115200 -D /dev/ttyUSB0
# Ctrl+A → O → Serial port setup → F (Hardware Flow Control: No)
```

---

## Troubleshooting

| Symptom | Cause | Solution |
|---------|-------|----------|
| No serial output | Wrong UART in bootargs | Use `console=ttyS0,115200` with `enable_uart=1` in config.txt |
| Output but no input | Hardware flow control enabled | Disable in minicom: `Ctrl+A → O → F` |
| Output but no input | TX/RX wires swapped | Swap TX and RX on the adapter side |
| `can't open /dev/ttyS0` | devtmpfs not mounted | Add `mount -t devtmpfs devtmpfs /dev` as first line in `rcS` |
| `No working init found` | Missing `/sbin/init` | Check rootfs structure, ensure busybox is installed |
| `exitcode=0x00007f00` | Missing shared libraries | Copy libs from toolchain sysroot or compile with `-static` |
| NFS mount fails | Wrong export path or IP | Verify `/etc/exports`, run `exportfs -v`, check IP with `ip addr` |
| NetworkManager changes IP | NM overrides manual config | Run `sudo nmcli device set eno1 managed no` |
| Initramfs wrong paths | `find` run from wrong directory | Must `cd` into rootfs before running `find . \| cpio ...` |

---

## Boot Flow

```
Power On
  ↓
bootcode.bin (GPU loads from SD)
  ↓
start.elf (GPU firmware)
  ↓
config.txt → loads u-boot.bin (kernel=u-boot.bin)
  ↓
U-Boot → executes boot.scr
  ↓
Load Image + DTB + initramfs (from SD/TFTP)
  ↓
Linux Kernel boots
  ↓
Mount rootfs (SD / NFS / initramfs)
  ↓
Run /sbin/init (BusyBox)
  ↓
/etc/init.d/rcS → mount filesystems
  ↓
Interactive shell on ttyS0
```

---

## License

This project is for educational purposes as part of the ITI Embedded Linux track.