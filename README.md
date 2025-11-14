## 📁 Project Structure (Module Overview)

| **Component**     | **Main Files**                         | **Module Description**                                                                 |
|-------------------|----------------------------------------|-----------------------------------------------------------------------------------------|
| **Core**          | `src/main.c`                           | System initialization, module orchestration, and main boot sequence.                    |
| **File System**   | `src/fs.c`, `include/fs.h`             | ROMFS-like filesystem image parser and directory traversal utilities.                   |
| **Memory Manager**| `src/mem.c`, `include/mem.h`           | Custom heap allocator implementation (`memalloc` / `memfree`) with coalescing.          |
| **Threading**     | `src/threads.c`, `include/threads.h`   | Cooperative user-level thread scheduler and manual context switching logic.             |
| **Protection**    | `src/protect.c`, `include/protect.h`   | Signal configuration and access control using `mprotect` to simulate memory isolation.  |
| **Sandboxing**    | `src/sandbox.c`, `include/sandbox.h`   | Process management using `fork`, IPC via `pipe`, and syscall filtering via `seccomp`.   |
| **Headers**       | `include/`                              | Shared `.h` header interfaces for cross-module linkage.                                 |

---

### 🧵 Threading & Inter-Module Communication Architecture

MiniOS implements a **cooperative user-level threading model**, where context switching is explicitly controlled by the scheduler. Within this framework, two primary threads coordinate user interaction and internal OS execution behavior:

- **Terminal Interface Thread (Input Handling Thread)**  
  Responsible for capturing user input from the terminal. This thread reads user commands, serializes them, and writes them into the communication channel.

- **Kernel Execution Thread (Worker / Processing Thread)**  
  Receives serialized commands from the Terminal Interface Thread via **UNIX pipes**, backed by **shared memory buffers** and **file descriptors**.  
  This thread acts as the “user-level kernel simulator,” interpreting the commands and invoking the appropriate internal subsystem modules.

Communication between threads is facilitated using:

- **POSIX `pipe()`** for event-driven message streaming.
- **File Descriptors** for endpoint addressing.
- **Shared Memory Buffers** to store and forward command payloads.
- **Cooperative Scheduling**, ensuring deterministic and safe context switching.

This design models **Inter-Thread Communication (ITC)** and simulates the request/dispatch behavior present in real kernel-user boundary interactions while operating purely in user space.

---

### 🖥️ Supported User-Level Commands

Once MiniOS is running, users may interact with the simulated kernel through the terminal interface thread.  
The kernel execution thread interprets and processes these commands:

| **Command** | **Description** |
|-------------|----------------|
| `ls` | Lists directory entries in the mounted ROMFS filesystem. |
| `+int,int` | Adds two integers and prints the result. |
| `-int,int` | Subtracts the second integer from the first and prints the result. |
| `find / -name "document_name.txt"` | Performs a filesystem-level search. |
| `server` | Launches a cooperative TCP server thread that listens on port 8888 and replies “I got your message”. |


## 🛠️ Build and Run

### Prerequisites
- **GCC or Clang** — C compiler suite.  
- **make** utility.  
- **Linux operating system** — required for `mprotect`, `seccomp`, and POSIX signal support.


### Compilation and Execution

```bash
make clean
make
./program
```
**Example:**
```bash
ls
+4,7
-10,3
find / -name "message.txt"
