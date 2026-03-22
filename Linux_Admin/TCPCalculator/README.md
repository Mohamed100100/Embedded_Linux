
# 🧮 Simple TCP Client-Server Sum Program

This project demonstrates a simple **TCP communication** between a client and a server using **sockets in C**.
The client sends **two integer numbers** to the server, and the server responds with their **sum**.

---

## 📂 Files

| File       | Description                                                                                                                                 |
| ---------- | ------------------------------------------------------------------------------------------------------------------------------------------- |
| `server.c` | Implements the TCP server. It listens for incoming client connections, receives two numbers, computes their sum, and sends the result back. |
| `client.c` | Implements the TCP client. It connects to the server, takes two numbers from the user, sends them, and displays the result.                 |

---

## ⚙️ How to Build

You need a Linux system (or WSL on Windows) with `gcc` installed.

Compile both files:

```bash
gcc server.c -o server
gcc client.c -o client
```

This will produce two executables:

* `server`
* `client`

---

## 🚀 How to Run

### Step 1: Start the Server

Run the server first, specifying **IP address** and **port** to bind:

```bash
./server 127.0.0.1 8080
```

Expected output:

```
The Socket is created Successfully
The Server is binding to 127.0.0.1:8080
Server listening...
```

---

### Step 2: Run the Client

In another terminal, run the client and connect to the same IP and port:

```bash
./client 127.0.0.1 8080
```

Expected interaction:

```
The Socket is created Successfully
Connected Successfully to 127.0.0.1:8080
Enter first number: 5
Enter second number: 7
Sum = 12
```

---

## 🧠 How It Works

1. The **server** creates a TCP socket, binds it to an IP and port, and listens for incoming connections.
2. The **client** creates a socket and connects to the server.
3. The client reads two numbers from the terminal and sends them to the server as strings.
4. The server receives both numbers, converts them from strings to integers using `atoi()`, sums them, converts the result back to a string using `sprintf()`, and sends it back.
5. The client receives the result and prints it to the terminal.

---

## 🧩 Notes

* You must use **the same IP and port** for client and server.
* The server must be running **before** you start the client.
* You can modify the IP and port from the command line arguments.
* The code uses **blocking I/O** (`read()` / `write()`).
* Both programs use **loopback address (127.0.0.1)** for local testing.

---

## 🧹 Example Run

**Terminal 1 (Server):**

```
$ ./server 127.0.0.1 9090
The Socket is created Successfully
The Server is binding to 127.0.0.1:9090
Server listening...
Client connected
```

**Terminal 2 (Client):**

```
$ ./client 127.0.0.1 9090
The Socket is created Successfully
Connected Successfully to 127.0.0.1:9090
Enter first number: 4
Enter second number: 9
Sum = 13
```

---

