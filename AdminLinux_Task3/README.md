

# Embedded Linux Task 1 - Admin Linux

This repository contains the solutions and explanations for Task 1 related to Bash scripting, Linux commands, and shell behavior.

---

## Section 1: Bash Script to Update `.bashrc`

A script was created to check if the `.bashrc` file exists in the user's home directory. If it does, the script appends:

- A global environment variable `HELLO` with the value of `HOSTNAME`.
- A local variable `LOCAL` with the value of `whoami`.

The script also includes a command to open a new terminal at the end.

**Observation:**

```bash
echo $LOCAL
echo $HELLO
````

* The **global variable** `HELLO` is printed successfully because it is shared between parent and child processes.
* The **local variable** `LOCAL` is not found in the new terminal because local variables are not shared across processes.

---

## Section 2: Linux Commands and Bash Behavior

### 1. List User Commands

```bash
ls /usr/bin/ /bin/ /usr/local/bin/ > /tmp/commands.list
```

This lists all commands available in standard directories and redirects the output to `/tmp/commands.list`.

---

### 2. Display Date at Login and Change Prompt Permanently

**Login Message:**

Add to `~/.profile`:

```bash
echo "Welcome $(whoami)! Today is $(date)"
```

* This prints a welcome message with the current date and username when the user logs in.

**Friendly Bash Prompt:**

Add to `~/.bashrc`:

```bash
# Friendly Bash Prompt
parse_git_branch() {
    git branch 2>/dev/null | grep '*' | sed 's/* //'
}

PS1="\[\e[1;36m\]😊 [\D{%H:%M:%S}]\[\e[0m\] "      # Cyan smile + time
PS1+="\[\e[1;32m\]\u\[\e[0m\]"                     # Green username
PS1+="\[\e[1;33m\]@\h\[\e[0m\] "                   # Yellow host
PS1+="\[\e[1;35m\]\w\[\e[0m\] "                    # Purple current directory
PS1+="\[\e[1;31m\]\$(parse_git_branch)\[\e[0m\] "  # Red Git branch
PS1+="\[\e[1;36m\]➜\[\e[0m\] "                     # Cyan arrow as prompt symbol
```

**Output Example:**

```
😊 [11:25:36] gemy@gemy-Precision-7530 /media/gemy/Linux_Workspace/ITI/Eng_Fady/Tasks/Embedded_Linux Task1_adminLinux ➜
```

---

### 3. Count Words and Files

* Number of words in a file:

```bash
wc -w <filename>
```

* Number of files in a directory:

```bash
ls -1 <directory> | wc -l
```

* Count number of user commands:

```bash
ls /usr/bin/ /bin/ /usr/local/bin/ | wc -w
```

> Note: Subtract 3 from the total because directory names are included in the count.

---

### 4. What happens if you execute:

**a. `cat filename1 | cat filename2`**

Now the `cat` on the left will take its stdin from the terminal, and its output will go through the stdin of the `cat` on the right, and the stdout of the `cat` on the right will be connected to the terminal.

So:

```
cat at left  >> stdin: terminal , stdout: stdin of the right cat
cat at right >> stdin: connected to the stdout of the left cat, stdout: terminal
```

What happens: the output will be the content of `filename2` only.

Explanation: `cat` first checks if it has an argument (file). If yes, it reads from the file and ignores stdin.

* The `cat` on the left reads `filename1` and sends it to the right `cat`.
* The `cat` on the right already has an argument `filename2`, so it reads from it and ignores the stdin (from `filename1`).

---

**b. `ls | rm`**

Now `ls` will take input from the terminal and send its output to the stdin of `rm`. The `rm` command’s stdin is connected to the output of `ls`, and its output goes to the terminal.

Result:

```
rm: missing operand
Try 'rm --help' for more information.
```

Explanation: `rm` deletes files that are passed as arguments, not from stdin. It ignores stdin and waits for file arguments, so nothing happens.

---

**c. `ls /etc/passwd | wc -l`**

```
ls >> stdin: terminal , stdout: stdin of wc
wc >> stdin: output of ls, stdout: terminal
```

The output of `ls /etc/passwd` will be `/etc/passwd`.
`wc` takes this input and counts the number of lines.

**Result:** `1`

---


### 5. Search for `.profile` Files

```bash
find / -iname .profile
```

---

### 6. List Inode Numbers

```bash
ls -id / /etc /etc/hosts
```

**Example Output:**

```
2 /  
6553601 /etc  
6553765 /etc/hosts
```

---

### 7. Create Symbolic Link of `/etc/passwd` in `/boot`

```bash
sudo ln -s /etc/passwd /boot/passwd_softlink
```

* Output:

```
lrwxrwxrwx 1 root root 11 ﻥﻮﻓ  5 12:04 /boot/passwd_softlink -> /etc/passwd
```

---

### 8. Create Hard Link of `/etc/passwd` in `/boot`

```bash
ln /etc/passwd /boot/passwd_hardlink
```

* **It doesn’t work** because hard links must reside on the **same filesystem/partition** as the original file.

---

### 9. Secondary Prompt (`>`)

* Typing:

```bash
echo \
```

* Bash prints `>` — this is the **secondary prompt** (`PS2`) that appears when a command is incomplete.

* To change it from `>` to `:`:

```bash
PS2=": "
```

* **Explanation:** PS2 is used for multi-line commands, e.g., when a backslash `\` is used.

---



