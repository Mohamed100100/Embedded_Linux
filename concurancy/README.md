# 📄 README — Caps Lock Watcher (Multithreaded Linux)

---

## 📌 Overview

This project implements a **multithreaded Caps Lock state watcher** on Linux.

It monitors the Caps Lock LED state by reading:

```
/sys/class/leds/input4::capslock/brightness
```

Two threads are used:

* **Provider thread** → monitors the file and detects state changes
* **Consumer thread** → waits for notifications and prints the state

The threads communicate using:

* `std::mutex`
* `std::condition_variable`

This demonstrates:

✅ Thread synchronization
✅ Producer–Consumer pattern
✅ Linux sysfs usage
✅ Blocking waits (no busy waiting)
✅ Safe shared data access

---

---

# 📂 How it works

---

## 🔹 Linux sysfs file

Path:

```
/sys/class/leds/input4::capslock/brightness
```

Values:

| Value | Meaning  |
| ----- | -------- |
| 0     | Caps OFF |
| 1     | Caps ON  |

This file:

* behaves like a regular file
* does NOT block on read
* must be re-seeked before each read (`seekg(0)`)

---

---

# 🧵 Thread Design

---

## 1️⃣ Provider Thread (Watcher)

### Responsibility:

* Periodically read Caps Lock state
* Detect changes
* Notify consumer

### Steps:

```
loop:
   read file
   if changed:
       lock mutex
       update shared state
       set flag
       unlock
       notify consumer
   sleep(50ms)
```

---

---

## 2️⃣ Consumer Thread (Waiter)

### Responsibility:

* Wait until notified
* Print state

### Steps:

```
loop:
   wait(condition_variable)
   print state
```

The thread **blocks** using `cv.wait()` and consumes **0% CPU** while waiting.

---

---

# 🔐 Synchronization Strategy

---

## Shared variables

```cpp
char capsLockState;
bool flag;
```

Protected by:

```cpp
std::mutex mx;
```

---

## Why condition_variable?

Without it:

```
while(!flag) {}
```

❌ Busy wait
❌ 100% CPU

With `condition_variable`:

```
sleep until notify
```

✅ efficient
✅ kernel blocking
✅ best practice

---

---

# 🧠 Design Pattern

This program follows the classic:

# Producer–Consumer Pattern

| Role     | Thread                  |
| -------- | ----------------------- |
| Producer | CapsLockWatcherCallable |
| Consumer | CapsLockWaiterCallable  |

Flow:

```
File change → Producer → notify → Consumer prints
```

---

---

# 🛠 Build Instructions

---

## Compile

```bash
g++ -std=c++17 CapsLockWatcher.cpp -pthread -o capswatch
```

`-pthread` is required for:

* threads
* mutex
* condition_variable

---

---

## Run

```bash
./capswatch
```

---

---

# ▶ Example Output

```
Provider CallBack Thread is created
Consumer CallBack Thread is created
Caps Lock : ON
Caps Lock : OFF
Caps Lock : ON
```

(Output updates whenever Caps Lock key is pressed)

---

---

# ⚙ Key C++ Concepts Used

---

## Concurrency

* `std::thread`
* `std::mutex`
* `std::unique_lock`
* `std::condition_variable`

---

## File handling

* `std::ifstream`
* `seekg()`
* `clear()`

---

## Timing

* `std::this_thread::sleep_for`

---

---

# 🚀 Performance Notes

---

## CPU usage

* Consumer → 0% (blocked on cv)
* Provider → very low (50ms polling)

---

## Why sleep?

Because:

```
sysfs files cannot block
```

So we must poll periodically.

---

## Better alternative (advanced)

For production Linux systems:

Use:

```
inotify
```

to get kernel notifications instead of polling.

---

---

# 📌 Possible Improvements

* Replace polling with `inotify`
* Graceful shutdown support
* Use atomic<bool> for flag
* Add logging
* Support multiple LEDs

---

---

# 📚 Learning Objectives

This task demonstrates:

✅ Thread synchronization
✅ Mutex protection
✅ Condition variables
✅ Avoiding busy waiting
✅ Linux sysfs interaction
✅ Producer–Consumer model

---

---

# ✅ Summary

This project efficiently monitors Caps Lock state using:

* two threads
* safe shared memory
* condition variables
* minimal CPU usage

It is a clean example of **modern C++ concurrency in Linux systems programming**.

---


