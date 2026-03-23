# ==============================================
# RPi3 Flexible Auto Boot Script (U-Boot)
# ==============================================

echo "=============================="
echo " RPi3 Flexible Boot Script"
echo "=============================="

# --------------------------------------------------
# 🔧 User Configuration (EDIT ONLY THIS PART)
# --------------------------------------------------

# Boot source selection:
# - "tftp" → load kernel + dtb over network using TFTP
# - "mmc"  → load kernel + dtb from SD card (FAT partition)
setenv boot_source tftp

# Root filesystem selection:
# - "nfs"       → rootfs mounted from NFS server (network rootfs)
# - "sdcard"    → rootfs from SD card (usually ext4 partition)
# - "initramfs" → rootfs is a RAM-based initramfs
setenv rootfs_type initramfs

# --------------------------------------------------
# Initramfs Load Address (in RAM)
# --------------------------------------------------
# This tells U-Boot where to load the initramfs image.
# Make sure it does not overlap with kernel or DTB.
setenv initramfs_addr_r 0x03000000

# --------------------------------------------------
# Step 1: Network Configuration
# --------------------------------------------------
# These are required for TFTP and NFS boot

# ipaddr     → IP address of the Raspberry Pi
# serverip   → IP address of the TFTP/NFS server
# gatewayip  → Network gateway (router)
# netmask    → Subnet mask

setenv ipaddr 192.168.1.100
setenv serverip 192.168.1.50
setenv gatewayip 192.168.1.1
setenv netmask 255.255.255.0

echo "Network configuration:"
echo "  IP: ${ipaddr}"
echo "  Server: ${serverip}"
echo "  Gateway: ${gatewayip}"

# --------------------------------------------------
# Step 2: Configure Kernel Boot Arguments (bootargs)
# --------------------------------------------------
# bootargs → passed from U-Boot to Linux kernel

if test "${rootfs_type}" = "nfs"; then
    echo "Using NFS root filesystem"

    setenv bootargs "
    console=ttyS0,115200 \
    8250.nr_uarts=1 \
    root=/dev/nfs \
    rw \
    nfsroot=${serverip}:/media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/sd_card/rootfs,v3,tcp \
    ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}::eth0:off \
    init=/sbin/init
    "

elif test "${rootfs_type}" = "initramfs"; then
    echo "Using initramfs as root filesystem"

    # For initramfs, kernel will mount RAM-based rootfs automatically
    setenv bootargs "
    console=ttyS0,115200 \
    8250.nr_uarts=1 \
    loglevel=8 \
    panic=5 \
    rdinit=/sbin/init
    "

else
    echo "Using SD card root filesystem"

    setenv bootargs "
    console=ttyS0,115200 \
    8250.nr_uarts=1 \
    loglevel=8 \
    panic=5 \
    root=/dev/mmcblk0p2 \
    rootfstype=ext4 \
    rw \
    rootwait \
    init=/sbin/init
    "
fi

# --------------------------------------------------
# Step 3: Kernel, Device Tree, and Initramfs Files
# --------------------------------------------------

# bootfile → Linux kernel image
# fdtfile  → Device Tree Blob (hardware description)
# initramfs_file → RAM-based root filesystem (U-Boot image)
setenv bootfile Image
setenv fdtfile bcm2710-rpi-3-b-plus.dtb
setenv initramfs_file uInitramfs  # Only used if rootfs_type=initramfs

# --------------------------------------------------
# Step 4: Boot Logic
# --------------------------------------------------

if test "${boot_source}" = "tftp"; then
    echo "=== Booting from TFTP ==="

    # Load kernel from TFTP server into RAM
    if tftp ${kernel_addr_r} ${bootfile}; then
        echo "Kernel loaded via TFTP"

        # Load device tree
        if tftp ${fdt_addr_r} ${fdtfile}; then
            echo "DTB loaded via TFTP"

            # Load initramfs if requested
            if test "${rootfs_type}" = "initramfs"; then
                if tftp ${initramfs_addr_r} ${initramfs_file}; then
                    echo "Initramfs loaded via TFTP"
                    booti ${kernel_addr_r} ${initramfs_addr_r} ${fdt_addr_r}
                else
                    echo "TFTP: Failed to load initramfs"
                fi
            else
                # Boot kernel without initramfs
                booti ${kernel_addr_r} - ${fdt_addr_r}
            fi

        else
            echo "TFTP: Failed to load DTB"
        fi
    else
        echo "TFTP: Failed to load Kernel"
    fi

else
    echo "=== Booting from MMC ==="

    # Load kernel from SD card (FAT partition)
    if fatload mmc 0:1 ${kernel_addr_r} ${bootfile}; then
        echo "Kernel loaded from MMC"

        # Load device tree
        if fatload mmc 0:1 ${fdt_addr_r} ${fdtfile}; then
            echo "DTB loaded from MMC"

            # Load initramfs if requested
            if test "${rootfs_type}" = "initramfs"; then
                if fatload mmc 0:1 ${initramfs_addr_r} ${initramfs_file}; then
                    echo "Initramfs loaded from MMC"
                    booti ${kernel_addr_r} ${initramfs_addr_r} ${fdt_addr_r}
                else
                    echo "MMC: Failed to load initramfs"
                fi
            else
                # Boot kernel without initramfs
                booti ${kernel_addr_r} - ${fdt_addr_r}
            fi

        else
            echo "MMC: Failed to load DTB"
        fi
    else
        echo "MMC: Failed to load Kernel"
    fi
fi

# --------------------------------------------------
# Step 5: Fallback Mechanism
# --------------------------------------------------

echo "Primary boot failed, trying fallback..."

if test "${boot_source}" = "tftp"; then
    echo "Fallback → MMC"

    if fatload mmc 0:1 ${kernel_addr_r} ${bootfile}; then
        if fatload mmc 0:1 ${fdt_addr_r} ${fdtfile}; then
            if test "${rootfs_type}" = "initramfs"; then
                if fatload mmc 0:1 ${initramfs_addr_r} ${initramfs_file}; then
                    booti ${kernel_addr_r} ${initramfs_addr_r} ${fdt_addr_r}
                fi
            else
                booti ${kernel_addr_r} - ${fdt_addr_r}
            fi
        fi
    fi

else
    echo "Fallback → TFTP"

    if tftp ${kernel_addr_r} ${bootfile}; then
        if tftp ${fdt_addr_r} ${fdtfile}; then
            if test "${rootfs_type}" = "initramfs"; then
                if tftp ${initramfs_addr_r} ${initramfs_file}; then
                    booti ${kernel_addr_r} ${initramfs_addr_r} ${fdt_addr_r}
                fi
            else
                booti ${kernel_addr_r} - ${fdt_addr_r}
            fi
        fi
    fi
fi

echo "All boot methods failed!"