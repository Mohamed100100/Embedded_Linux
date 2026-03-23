
### **1. What is initramfs? Why use it instead of mounting the real rootfs directly?**

**initramfs (Initial RAM Filesystem)** is a temporary root filesystem loaded into RAM during the early Linux boot process.

🔹 **Why we use it:**

* The kernel **doesn’t yet know how to access your real rootfs**
* Needed to:

  * Load drivers (e.g., MMC, USB, SATA)
  * Initialize hardware
  * Set up filesystems (ext4, etc.)
* Acts as a **bridge** between kernel start and real rootfs

💡 Without initramfs → kernel may fail to mount `/dev/mmcblk0p2` because the driver isn’t loaded yet.

### **2. Why cpio format for initramfs? Why not tar or zip?**

**cpio** is used because:

✅ Simple structure → easy for kernel to parse
✅ No need for full filesystem support
✅ Supports:

* Device files
* Permissions
* Symlinks

🔻 Why NOT tar/zip:

* Require **more complex parsing logic**
* Kernel keeps things **minimal in early boot**
* zip needs decompression libraries → too heavy

💡 Kernel has a **built-in cpio extractor**

---

### **3. What does `rdinit=` do? What happens if wrong path?**

`rdinit=` tells the kernel:

👉 **Which program to run as the first process (PID 1) inside initramfs**

Example:

```bash
rdinit=/init
```

🔻 If the path is wrong:

* Kernel **can’t find init**
* Boot fails with:

  ```
  No init found. Try passing init= option to kernel.
  Kernel panic!
  ```

💡 This is a **critical parameter**

---

### **4. Why must init be statically linked? What if dynamic?**

Inside initramfs:

* No shared libraries exist (`/lib`, `/usr/lib` may not be ready)

🔹 So:
👉 `init` must be **statically linked**

🔻 If dynamically linked:

* It will try to load libraries → **fail**
* Result:

  ```
  No such file or directory
  ```

  (even if the file exists!)

💡 Classic debugging trap ⚠️

---

### **5. Difference: initramfs vs initrd?**

| Feature        | initramfs           | initrd             |
| -------------- | ------------------- | ------------------ |
| Type           | CPIO archive in RAM | Block device image |
| Mounting       | No mount needed     | Must be mounted    |
| Flexibility    | More flexible       | Less flexible      |
| Kernel support | Modern (used today) | Older method       |

💡 initramfs **replaced initrd** in modern Linux

---

### **6. Where is initramfs loaded in memory? Who decompresses it?**

🔹 Loaded into:
👉 **RAM** (as part of kernel boot process)

🔹 Who decompresses it?
👉 **The Linux kernel itself**

Steps:

1. Bootloader (e.g., U-Boot) loads:

   * Kernel image
   * initramfs
2. Kernel:

   * Decompresses initramfs
   * Extracts it into a **tmpfs/ramfs**

---

### **7. How does kernel switch from initramfs to real rootfs?**

This is done **manually by init (you)** 👇

🔹 Steps:

1. Mount real rootfs:

   ```bash
   mount /dev/mmcblk0p2 /new_root
   ```

2. Switch root:

   ```bash
   switch_root /new_root /sbin/init
   ```

   or:

   ```bash
   pivot_root
   ```

3. New `/sbin/init` becomes PID 1

🔹 Result:
👉 System continues boot from **real rootfs**

💡 After switching → initramfs is discarded

---

### 🔥 Quick Summary (important for interviews)

* initramfs = **temporary rootfs in RAM**
* Used to **prepare system before real rootfs**
* Uses **cpio (simple + kernel-friendly)**
* `rdinit=` → defines first process
* init must be **static**
* initramfs **replaces initrd**
* Kernel **loads + extracts it**
* Switching done via **switch_root**

---
