
## **1. Monolithic vs Microkernel — Where does Linux stand?**

### 🔹 Monolithic Kernel

* All services run in **kernel space**

  * Drivers
  * Filesystems
  * Networking
* High performance (no IPC overhead)

### 🔹 Microkernel

* Only minimal core in kernel:

  * Scheduling
  * IPC
  * Memory management
* Everything else in **user space**

### ✅ Linux:

👉 **Monolithic kernel (with modular support)**

* Supports loadable kernel modules (LKMs)
* So sometimes called **“modular monolithic”**

---

## **2. Why embedded devices use Linux instead of QNX (microkernel)?**

Even though **QNX** is real-time and safer:

### 🔹 Reasons Linux dominates:

* ✅ **Cost** → Linux is free, QNX is commercial
* ✅ **Huge ecosystem**
* ✅ **Driver support** (critical for hardware)
* ✅ **Community + updates**
* ✅ **Flexibility**

### 🔻 Real-time issue?

* Linux can be made **real-time** using:

  * PREEMPT_RT patch

💡 So companies prefer:
👉 “Good enough real-time + huge ecosystem” over perfect RTOS

---

## **3. What is Android GKI? Why enforced from Android 13?**

### 🔹 Android GKI (Generic Kernel Image)

* A **standardized Linux kernel** provided by Google
* Vendors **must not modify core kernel**

### 🔹 Why Google enforced it:

* Fix **fragmentation problem**
* Faster **security updates**
* Easier **Android upgrades**

### 🔹 Vendors now:

* Put custom code in:

  * **Kernel modules only**

💡 Result:
👉 System becomes **modular + maintainable**

---

## **4. Why clone raspberrypi/linux instead of torvalds/linux for RPi?**

### 🔹 Raspberry Pi Foundation kernel repo includes:

* Board-specific patches
* GPU support (VideoCore)
* Custom drivers

### 🔻 Linux kernel project:

* Generic kernel
* May **lack RPi-specific support**

💡 So:
👉 Use RPi repo → “it just works”

---

## **5. Kernel Images Difference**

| Image        | Description                         |
| ------------ | ----------------------------------- |
| **vmlinux**  | Uncompressed ELF (debugging)        |
| **zImage**   | Compressed kernel (self-extracting) |
| **Image**    | Raw uncompressed binary             |
| **uImage**   | U-Boot wrapped image (with header)  |
| **Image.gz** | Compressed Image                    |

💡 Flow:

```
vmlinux → (compress) → zImage / Image.gz → (wrap) → uImage
```

---

## **6. Why `fdt_addr_r`? What is DTB?**

### 🔹 DTB (Device Tree Blob)

* Binary describing hardware:

  * CPU
  * Memory
  * Peripherals

### 🔹 `fdt_addr_r`

* Memory address where DTB is loaded

💡 Kernel reads DTB to understand hardware
👉 Without it → kernel is “blind”

---

## **7. Explain bootargs**

Example:

```bash
root=/dev/mmcblk0p2 rootfstype=ext4 console=ttyAMA0 init=/init
```

### 🔹 Meaning:

* `root=` → root filesystem device
* `rootfstype=` → filesystem type
* `console=` → where logs go (UART)
* `init=` → first user-space program

💡 Passed from bootloader → kernel

---

## **8. Why `bootz` for ARM32, `booti` for ARM64?**

* `bootz` → boots **zImage** (compressed, ARM32)
* `booti` → boots **Image** (ARM64 expects raw Image)

💡 Architecture difference:

* ARM32 → zImage standard
* ARM64 → Image standard

---

## **9. What causes "VFS: Unable to mount root fs"?**

### 🔥 Common reasons:

* Wrong `root=` device
* Missing driver (e.g., MMC not enabled)
* Wrong filesystem type
* Corrupted rootfs

💡 Kernel cannot access storage → panic

---

## **10. Why custom init.c needs `-static`? What if not?**

### 🔹 Must be static because:

* No shared libraries available early boot

### 🔻 If not static:

* Dynamic linker missing (`ld-linux.so`)
* Program fails → kernel panic

---

## **11. You passed `init=/bin/sh` but still panics. Why?**

### 🔥 Possible reasons:

* `/bin/sh` doesn’t exist
* Not executable
* Missing libraries (dynamic linking issue)
* Rootfs not mounted correctly

💡 Most common:
👉 `/bin/sh` is **dynamically linked**

---

## **12. Why must init be statically linked? (again)**

### 🔹 Summary:

* Early boot = no `/lib`
* No dynamic loader
* No dependency resolution

### 🔻 If you forget `-static`:

* Error:

  ```
  No such file or directory
  ```
* Even if file exists!

💡 Because:
👉 Kernel cannot load required `.so` files

---

## 🔥 Final Interview Tips

If asked quickly:

* Linux = **monolithic**
* initramfs = **temporary rootfs**
* DTB = **hardware description**
* GKI = **kernel standardization**
* `-static` = **no dependencies**
* Panic (VFS) = **rootfs issue**

---


