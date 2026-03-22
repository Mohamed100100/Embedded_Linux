
>> 1. create symbolic link for the u-boot and linux kernel repositories

realpath ../../../../Embedded_Linux_WS/Linux_Kernel/linux/
realpath ../../../../Embedded_Linux_WS/uboot/u-boot/
readpath ../../../../Embedded_Linux_WS/Busybox/busybox/

ln -s /media/gemy/Linux_Workspace/ITI/Eng_Fady/Embedded_Linux_WS/uboot/u-boot uboot_repo_Slink

ln -s /media/gemy/Linux_Workspace/ITI/Eng_Fady/Embedded_Linux_WS/Linux_Kernel/linux linux_repo_Slink

ln -s /media/gemy/Linux_Workspace/ITI/Eng_Fady/Embedded_Linux_WS/Busybox/busybox busybox_repo_Slink

>> 2. create the bootfs and rootfs directories to hold the files that will be copied to the sdcard
mkdir -p sd_card/bootfs
mkdir -p sd_card/rootfs

// copy the boot files to the boot partition
// i put it at /media/gemy/Linux_Workspace/ITI/Eng_Fady/Embedded_Linux_WS/rpi_boot_files

sudo cp /media/gemy/Linux_Workspace/ITI/Eng_Fady/Embedded_Linux_WS/rpi_boot_files/* sd_card/bootfs/

😊 [17:00:00] gemy@gemy-Precision-7530 /media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image Task1_adminLinux ➜ ls sd_card/bootfs/
bootcode.bin  cmdline.txt  config.txt  fixup4cd.dat  fixup4.dat  fixup4db.dat  fixup4x.dat  fixup_cd.dat  fixup.dat  fixup_db.dat  fixup_x.dat  start4.elf  start.elf


>> 3. copy the u-boot binary to the boot partition

sudo cp /media/gemy/Linux_Workspace/ITI/Eng_Fady/Embedded_Linux_WS/uboot_repo_Slink/u-boot.bin sd_card/bootfs/

>> 4. copy the image and dtbo files to the boot partition
// copy the linux kernel image to the boot partition
sudo cp /media/gemy/Linux_Workspace/ITI/Eng_Fady/Embedded_Linux_WS/linux_repo_Slink/arch/arm/boot/Image sd_card/bootfs/

// copy the dtbo files to the boot partition
sudo cp /media/gemy/Linux_Workspace/ITI/Eng_Fady/Embedded_Linux_WS/linux_repo_Slink/arch/arm/boot/dts/bcm2710-rpi-3-b-plus.dtb sd_card/bootfs/

>> 5. create boot script file to load the kernel and dtbo files using u-boot
// create the bootscript file using mkimage
alias mkimage=/media/gemy/Linux_Workspace/ITI/Eng_Fady/Embedded_Linux_WS/uboot/u-boot/tools/mkimage

sudo mkimage -A arm64 -T script -C none -n "Boot Script" -d boot.cmd boot.scr

// put the bootscript file in the boot partition
sudo cp boot.scr sd_card/bootfs/

>> 6. copy the root filesystem files to the root partition


// after i finish copying the files to the boot partition, i will put the sdcard in the card reader and by default it will be mounted 
// run lsblk to check the name of the sdcard and the partitions


sudo rsync -aHAX --progress sd_card/bootfs/ <name_of_sdcard_partition_for_boot>
sudo rsync -aHAX --progress sd_card/rootfs/ <name_of_sdcard_partition_for_root>


// after i finish copying the files, i will unmount the sdcard and eject it
sudo umount <name_of_sdcard_partition_for_boot>
sudo umount <name_of_sdcard_partition_for_root>



=====================================================================================================================
>> prepare to boot from the tftp

// you must put this line in the /etc/default/tftpd-hpa file to configure the tftp server to serve the files from the tftp directory and to allow creating new files in it
😊 [18:29:09] gemy@gemy-Precision-7530 ~  ➜ cat /etc/default/tftpd-hpa 
# /etc/default/tftpd-hpa

TFTP_USERNAME="tftp"
TFTP_DIRECTORY="/srv/tftp"
TFTP_ADDRESS=":69"
TFTP_OPTIONS="--secure --create"

install tftpd-hpa
sudo apt install tftpd-hpa

// after installing the tftp server, you must start it using the following command
in.tftpd --listen --user tftp --address :69 --secure --create /srv/tftp 
// systemctl restart tftpd-hpa

// put the Image and dtbo files in the tftp directory
sudo cp /media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/linux_repo_Slink/arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dtb /srv/tftp/
sudo cp /media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/linux_repo_Slink/arch/arm64/boot/Image /srv/tftp/

// set the ip address of the tftp server to the ip address of the host machine
 sudo ip addr add 192.168.1.50/24 dev eno1

======================================================================================================================
>> prepare the kernel image and the dtbo files


// you have two options to get the main repo of linus tervalds or you can get the linux kernel of certain board from the raspberry pi github repository
git clone --depth 1 https://github.com/raspberrypi/linux.git

// after cloning the linux kernel repository, you must configure it to build the kernel image and the dtbo files for the raspberry pi 3 b+ board
cd linux

// configure the kernel to build the image and the dtbo files for the raspberry pi 3 b+ board
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2710_defconfig
// after configuring the kernel, you can build the kernel image and the dtbo files using the following command
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc) Image dtbs
// now generate the kernel modules using the following command
make modules_install INSTALL_MOD_PATH=../sd_card/rootfs/

// after building the kernel image and the dtbo files, you can copy them to the tftp directory to be served by the tftp server
sudo cp arch/arm64/boot/Image /srv/tftp/
sudo cp arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dtb /srv/tftp/

OR 
// copy to the boot partition of the sdcard to be loaded by u-boot
sudo cp arch/arm64/boot/Image /media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/sd_card/bootfs/
sudo cp arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dtb /media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/sd_card/bootfs/

// copy the kernel modules to the root filesystem of the sdcard
sudo cp -r /media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/sd_card/rootfs/lib/modules/5.15.0-1018-raspi /media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/sd_card/rootfs/lib/modules/5.15.0-1018-raspi 



=======================================================================================================================
>> get the rootfs from nfs 
// @the host machine
sudo apt update
sudo apt install nfs-kernel-server

sudo vim /etc/exports
// add the following line to the /etc/exports file to export the rootfs directory to the network
/media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/sd_card/rootfs 192.168.1.100(rw,sync,no_subtree_check,no_root_squash)
// after adding the line to the /etc/exports file, you must restart the nfs server using the following command
/media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/sd_card/rootfs 192.168.1.100(rw,sync,no_subtree_check,no_root_squash)
sudo exportfs -ra
sudo exportfs -v

// change the uboot bootargs to use nfsroot instead of root=/dev/mmcblk0p2
setenv bootargs "console=ttyS0 8250.nr_uarts=1,115200 root=/dev/nfs rw nfsroot=192.168.1.50:/media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux/Embedded_Linux/Flash_Linux_image/sd_card/rootfs,v3,tcp ip=192.168.1.100:192.168.1.50:192.168.1.1:255.255.255.0::eth0:off init=/init"

// to make sure the server ip
sudo nmcli device set eno1 managed no
sudo ip addr flush dev eno1
sudo ip addr add 192.168.1.50/24 dev eno1
sudo ip link set eno1 up


=======================================================================================================================
>> prepare the rootfs files
// 1. copy the busybox files to the rootfs directory
cp -r busybox_repo_Slink/_install/sbin/ sd_card/rootfs
cp -r busybox_repo_Slink/_install/bin/ sd_card/rootfs
cp -r busybox_repo_Slink/_install/usr/ sd_card/rootfs

// 2. create the necessary directories in the rootfs directory
mkdir -p sd_card/rootfs/{dev,proc,sys,etc,home,tmp,}

// 3. copy the libs files to the rootfs directory
sudo cp -r ~/x-tools/aarch64-rpi3-linux-gnu/aarch64-rpi3-linux-gnu/sysroot/usr/lib sd_card/rootfs/usr/
cp -r ~/x-tools/aarch64-rpi3-linux-gnu/aarch64-rpi3-linux-gnu/sysroot/lib sd_card/rootfs/ 

// then create symbolic link for lib64 to point to lib using the following command
cd sd_card/rootfs
ln -s lib lib64
cd -

// after copying the libs files to the rootfs directory, you must change the ownership of the files to the root user using the following command
sudo chown -R root:root sd_card/rootfs/

// also the permissions of the files must be changed to 755 using the following command (755 means that the owner has read, write, and execute permissions, while the group and others have read and execute permissions)
sudo chmod -R 755 sd_card/rootfs/

// 4. create the init configuration file in the rootfs directory
sudo vim sd_card/rootfs/etc/inittab

// add the following lines to the /etc/inittab file to configure the init system to start the necessary services and to provide a login prompt on the console
// 1.run the rcS script to start the necessary services, sysinit is the first process that will be run by the kernel after it finishes loading, and it will run the rcS script to start the necessary services
::sysinit:/etc/init.d/rcS
// 2. run the shell on the console, askfirst means that the shell will be started on the console only if there is no other process running on the console, and ttyS0 is the first UART that we are using for the console output{you will press enter to get the login prompt on the console}
ttyS0::askfirst:-/bin/sh
// 3. configure the system to reboot when the user presses Ctrl+Alt+Del, providing a convenient way to restart the system if needed 
::ctrlaltdel:/sbin/reboot
// 4. configure the system to shut down when the user types "poweroff" or "halt", allowing for umounting the filesystems and performing any necessary cleanup before powering off the system
::shutdown:/bin/umount -a -r

// 5. create the rcS script in the rootfs directory
mkdir sd_card/rootfs/etc/init.d
sudo vim sd_card/rootfs/etc/init.d/rcS

#!/bin/sh
# mount -t devtmpfs devtmpfs /dev << happens automatically with devtmpfs, so we don't need to do it manually
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t tmpfs tmpfs /tmp
echo "System booted successfully!"




