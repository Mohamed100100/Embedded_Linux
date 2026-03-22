
# CapslockControl

`CapslockControl` is a small C program for Linux that allows you to **control the Caps Lock LED** by turning it ON or OFF using the command line.  

> **Note:** Root privileges are required to write to `/sys/class/leds/...`.

---

## Usage

```bash
sudo ./CapslockControl.exe 1   # Turn ON Caps Lock LED
sudo ./CapslockControl.exe 0   # Turn OFF Caps Lock LED
````

* `1` : Turns the Caps Lock LED ON
* `0` : Turns the Caps Lock LED OFF

If the program is run with **no arguments** or **invalid arguments**, it prints a usage message.

---

## Example

```bash
sudo ./CapslockControl.exe 1
# Output:
# it works successfully.
# Look at the CapsLock Led :)

sudo ./CapslockControl.exe 0
# Output:
# it works successfully.
# Look at the CapsLock Led :)
```

---

## Compilation

```bash
gcc CapslockControl.c -o CapslockControl.exe
```

Make sure to include your custom headers if they are in a `LIB` folder:

```bash
#include "./LIB/errtype.h"
#include "./LIB/stdtypes.h"
```

---

## Implementation Notes

* The program writes directly to the **Caps Lock LED brightness file** located at:

```
/sys/class/leds/input4::capslock/brightness
```


---

## Permissions

Writing to `/sys/class/leds/...` requires **root permissions**, so always run the program with `sudo`.

---

