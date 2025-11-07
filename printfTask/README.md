

# 🧩 Low-Level Syscall Write Example

## 📘 Overview

This project demonstrates how to **invoke the Linux `write()` system call directly using inline assembly**, without relying on the C standard library.
It supports multiple architectures, including:

* **x86_64 (64-bit x86)**
* **x86 (32-bit)**
* **ARM (32-bit)**
* **ARM64 (AArch64)**

The program defines two functions:

1. **`my_write()`** — performs a direct syscall to write data to a file descriptor.
2. **`myPrintf()`** — simple print function that calculates string length and calls `my_write()` to display text on the console (stdout).

---

## 📂 File Structure

```
├── main.c         # Test application that calls myPrintf()
├── myWrite.c      # Contains syscall implementation for different architectures
├── myWrite.h      # Header declaring my_write() and myPrintf()
```

---

## ⚙️ Build Instructions

To compile the program:

```bash
gcc main.c myWrite.c -o testWriteSyscall.exe
```

You can also specify an architecture manually (if cross-compiling):

```bash
# For ARM64
aarch64-linux-gnu-gcc main.c myWrite.c -o testWriteSyscall_arm64
```

---

## 🚀 Run

```bash
./testWriteSyscall.exe
```

**Expected output:**

```
hello world 
size = 13
```

---

## 🧠 Explanation

* `my_write()` sets up syscall arguments in CPU registers according to the Linux syscall ABI for the current architecture.
* It executes the proper **trap instruction**:

  * `syscall` → x86_64
  * `int $0x80` → x86
  * `swi 0` → ARM
  * `svc #0` → ARM64
* The return value is the number of bytes written (like the standard `write()`).

---

## 🔍 Notes

* This code runs **only on Linux**, because syscall numbers and calling conventions are Linux-specific.
* `SYS_write` is provided by `<sys/syscall.h>`.
* `myPrintf()` mimics `printf()` behavior but does **not** format text — it only writes raw strings.

---

