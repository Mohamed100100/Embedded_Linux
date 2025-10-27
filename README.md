# 🐧 Linux Commands and Exercises

## 1️⃣ List three Linux Distributions
ubuntu , RedHat , Kali linux



---

## 2️⃣ From the slides, what is the man command used for?
the man command is used for display the documentation to help you to deal with commands , system calls , C Lib functons ,.... and for example it help you with using command with different options and also for systemcalls how to use it in your code



---

## 3️⃣ What is the difference between rm and rmdir using man command?
rm >> removes each specified file. By default, it does not remove directories.

rmdir >> remove empty directories



---

## 4️⃣ Create the following hierarchy under your home directory
```
├── dir1 
│   ├── dir11
│   │ └── file1
│   └── dir12
├── docs
│   └── mycv
├── Doucments
│   └── OldFiles
```

### a️. Remove dir11 with rmdir in one-step  
i notice that it doesn't work because rmdir works with empty dirctory , i can solve it using "rm -r"


### b️. Then remove OldFiles using rmdir –p command  
mdir: failed to remove 'Doucments/OldFiles': Not a directory

### c️. Write the absolute and relative path for the file mycv  
- absluote Path : /media/gemy/Linux_Workspace/ITI/Eng_Fady/Day1_question/Embedded_Linux/docs\
- relative path : ./docs

---

## 5️⃣ Copy the /etc/passwd file to your home directory making its name mypasswd  
## 6️⃣ Rename this new file to be oldpasswd  
`cp /etc/passwd  /etc/mypasswd`


---

## 7️⃣ You are in /usr/bin, list four ways to go to your home directory
```
cd ~
cd /home/gemy/
cd
cd ../../home/gemy/
```
---

## 8️⃣ List Linux commands in /usr/bin that start with letter w
```ls /usr/bin/w*```



**Output:**
```
/usr/bin/w           /usr/bin/wdctl    /usr/bin/which.debianutils  /usr/bin/whoopsie-preferences
/usr/bin/wall        /usr/bin/wget     /usr/bin/whiptail           /usr/bin/word-list-compress
/usr/bin/watch       /usr/bin/whatis   /usr/bin/who                /usr/bin/wpa_passphrase
/usr/bin/watchgnupg  /usr/bin/whereis  /usr/bin/whoami             /usr/bin/write
/usr/bin/wc          /usr/bin/which    /usr/bin/whoopsie           /usr/bin/write.ul

```
