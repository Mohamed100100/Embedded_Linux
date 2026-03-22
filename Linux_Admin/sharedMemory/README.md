

# 🧠 Shared Memory IPC — Sender & Receiver Example

## 📘 Overview

This project demonstrates **inter-process communication (IPC)** using **System V Shared Memory** on Linux.
It consists of two programs:

* **Sender:** reads input from the terminal and writes it to a shared memory segment.
* **Receiver:** continuously monitors the same shared memory segment and prints received messages to the console.

Both processes use a simple **synchronization flag** (`buf[0]`) to signal when data is ready.

---

## 📂 Files

```
├── sender.c     # Reads input and writes to shared memory
├── receiver.c   # Reads from shared memory and prints to stdout
```

---

## ⚙️ Build

Compile both programs:

```bash
gcc sender.c   -o sender.exe
gcc receiver.c -o receiver.exe
```

---

## 🚀 Run

1. **Start the receiver** first (to listen for messages):

   ```bash
   ./receiver.exe
   ```

   Example output:

   ```
   [Receiver] Shared memory attached at address: 0x7f8c56c00000
   ```

2. **Start the sender** in another terminal:

   ```bash
   ./sender.exe
   ```

   Example output:

   ```
   [Sender] Shared memory attached at address: 0x7f8c56c00000
   ```

3. Now type a message in the **sender** terminal.
   The **receiver** will instantly print it when `buf[0]` becomes `1`.

---

## 🧩 How It Works

* Both processes connect to the same shared memory segment using:

  ```c
  shmget(SHM_KEY, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
  ```
* `buf[0]` acts as a **flag**:

  * `0` → No data
  * `1` → New data available
* `buf[1..]` contains the message data.

### Sender logic:

```c
read(0, buf + 1, SHARED_MEMORY_SIZE - 1);
buf[0] = 1;  // Mark data as ready
```

### Receiver logic:

```c
if (buf[0] == 1) {
    write(1, buf + 1, SHARED_MEMORY_SIZE - 1);
    memset(buf + 1, '\0', SHARED_MEMORY_SIZE - 1);
    buf[0] = 0;  // Mark as consumed
}
```

---




