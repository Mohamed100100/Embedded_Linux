
# Lab Exercise – Shell and Environment Variables

## Question 2

**Task:**
Move the binary file output to the directory `/usr/local/bin` with sudo permissions.
Afterward, attempt to execute the binary from any working directory and explain the outcome. Provide a detailed explanation supported by evidence as to why the binary can be executed from any location.

**Answer:**
It runs because I moved the binary to the `/usr/bin` path, and this path is located in the `PATH` environment variable.

---

## 1: List the available shells in your system

```bash
>> cat /etc/shells

```

**Output:**

```
/bin/sh
/bin/bash
/usr/bin/bash
/bin/rbash
/usr/bin/rbash
/usr/bin/sh
/bin/dash
/usr/bin/dash
```

---

## 2: List the environment variables in your current shell

```bash
>> env
```

**Output (partial):**

```
SHELL=/bin/bash
SESSION_MANAGER=local/gemy-Precision-7530:@/tmp/.ICE-unix/2484,unix/gemy-Precision-7530:/tmp/.ICE-unix/2484
QT_ACCESSIBILITY=1
COLORTERM=truecolor
XDG_CONFIG_DIRS=/etc/xdg/xdg-ubuntu:/etc/xdg
SSH_AGENT_LAUNCHER=gnome-keyring
PWD=/media/gemy/Linux_Workspace/St_Workspace/myShell
LOGNAME=gemy
HOME=/home/gemy
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin
...
```

---

## 3: Display your current shell name

```bash
>> echo $SHELL
```

**Output:**

```
/bin/bash
```

---

## 4: Execute the following command: `echo \` then press Enter

**Observation:**
It helps me to execute the command in many lines.

**Example:**

```bash
ls \
> /
bin   cdrom  etc   lib    lib64   lost+found  mnt  proc  run   snap  sys  usr
boot  dev    home  lib32  libx32  media       opt  root  sbin  srv   tmp  var
```

---

## 5: Create a Bash shell alias

**Command:**

```bash
alias PrintPath="echo $PATH"
```

**Purpose:**
The alias `PrintPath` can be used to quickly display the current `PATH` environment variable.

---

