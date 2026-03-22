# ==============================================
# RPi3 Auto Boot Sequence Script (U-Boot)
# ==============================================

echo "=============================="
echo " RPi3 Auto Boot Sequence"
echo "=============================="

# --------------------------------------------------
# Step 1: Set Kernel Command Line Arguments
# --------------------------------------------------
# These arguments will be passed to the Linux kernel on boot. Adjust as needed for your setup.
# - console         = ttyS0,115200 8250.nr_uarts=1: use the first UART (ttyS0) for console output at 115200 baud, and specify that there is only 1 UART to avoid probing for additional ones.
# - loglevel        = 8: Set maximum log level (debug).
# - panic           = 5: Reboot after 5 seconds if the kernel panics.
# - root            = /dev/mmcblk0p2: Root filesystem is on the second partition of the SD card.
# - rootfstype      = ext4: The root filesystem type is ext4.
# - rw:              Mount the root filesystem as read-write.
# - rootwait:        Wait for the root device to be ready before mounting.
# setenv bootargs "console=ttyS0,115200 8250.nr_uarts=1 loglevel=8 panic=5 root=/dev/mmcblk0p2 rootfstype=ext4 rw rootwait init=/init"


# For NFS root filesystem, use the following bootargs instead. Adjust the nfsroot path and server IP as needed.
# The nfsroot argument specifies the NFS server and path to the root filesystem. The ip argument configures the network settings for the Pi.
# root=/dev/nfs: Use NFS as the root filesystem.
# nfsroot=<server_ip>:<nfs_path>:v3,tcp: Configure NFS root with the server IP and path, using NFSv3 over TCP.
# ip=<client_ip>:<server_ip>:<gateway_ip>:<netmask>::eth0:off: Configure the network settings for the Pi, including client IP, server IP, gateway, netmask, and interface.
setenv bootargs "console=ttyS0,115200 8250.nr_uarts=1 root=/dev/nfs rw nfsroot=192.168.1.50:/media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/sd_card/rootfs,v3,tcp ip=192.168.1.100:192.168.1.50:192.168.1.1:255.255.255.0::eth0:off init=/sbin/init"



# --------------------------------------------------
# Step 2: Set Kernel and Device Tree filenames
# --------------------------------------------------
setenv bootfile Image
setenv fdtfile bcm2710-rpi-3-b-plus.dtb

# Save environment variables to persistent storage
saveenv

# --------------------------------------------------
# Step 3: Configure Network for TFTP Boot
# --------------------------------------------------
# Set your Pi's IP address and netmask
setenv ipaddr 192.168.1.100          # Pi's static IP on the network
setenv netmask 255.255.255.0         # Network mask
# Set TFTP server IP
setenv serverip 192.168.1.50         # TFTP server IP

# Optional: set gateway if needed
setenv gatewayip 192.168.1.1

# Print network info
echo "Network configuration:"
echo "  IP: ${ipaddr}"
echo "  Netmask: ${netmask}"
echo "  Gateway: ${gatewayip}"
echo "  TFTP Server: ${serverip}"

# --------------------------------------------------
# Step 4: Attempt TFTP Boot
# --------------------------------------------------
echo "=== Attempting TFTP Boot ==="
if tftp ${kernel_addr_r} ${bootfile}; then
    echo "Kernel loaded via TFTP"

    if tftp ${fdt_addr_r} ${fdtfile}; then
        echo "DTB loaded via TFTP"
        booti ${kernel_addr_r} - ${fdt_addr_r}
    else
        echo "TFTP: Failed to load DTB"
    fi
else
    echo "TFTP: Failed to load Kernel"
fi

# --------------------------------------------------
# Step 5: Attempt MMC Boot (Fallback)
# --------------------------------------------------
echo "TFTP failed, trying MMC..."
echo "=== Attempting MMC Boot ==="
if fatload mmc 0:1 ${kernel_addr_r} ${bootfile}; then
    echo "Kernel loaded from MMC"

    if fatload mmc 0:1 ${fdt_addr_r} ${fdtfile}; then
        echo "DTB loaded from MMC"
        booti ${kernel_addr_r} - ${fdt_addr_r}
    else
        echo "MMC: Failed to load DTB"
    fi
else
    echo "MMC: Failed to load Kernel"
fi

echo "All boot methods failed!"