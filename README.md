# MiniOS: A Modular Operating System Simulator in C

## Overview
**MiniOS** is a modular simulator written in C that integrates fundamental concepts of operating systems into a cohesive and tangible project.  
It is designed as a learning tool to study and demonstrate topics such as memory management, file systems, threading, protection mechanisms, and inter-process communication.

Each module corresponds to a key concept explored throughout an undergraduate Operating Systems course, combined to form a small yet functional simulation of how an operating system operates internally.

## Features
- **File System Module (ROMFS Parser)**  
  Implements parsing and reading of a ROMFS-like filesystem image to simulate mounting and file exploration.
  
- **Memory Manager (Custom Allocator)**  
  Provides a simple implementation of `memalloc` and `memfree`, using a linked-list allocator with coalescence to manage heap memory.

- **Cooperative Thread Scheduler**  
  Simulates user-level cooperative threads using manual context switching, mimicking coroutine behavior and thread scheduling.

- **Memory Protection and Signal Handling**  
  Uses `mprotect` and POSIX signals to emulate isolation and error recovery between threads.

- **Secure Process Sandbox**  
  Demonstrates process isolation and inter-process communication using `pipe`, `fork`, and `seccomp` to restrict system calls.

## Project Structure
miniOS/
├── src/
│ ├── fs.c / fs.h # ROMFS parser and filesystem utilities
│ ├── mem.c / mem.h # Memory allocation and deallocation system
│ ├── threads.c / threads.h # Cooperative threading and scheduler
│ ├── protect.c / protect.h # Memory protection and signal handling
│ ├── sandbox.c / sandbox.h # Process sandboxing and IPC
│ └── main.c # System initialization and integration
├── include/ # Shared headers
├── Makefile # Build configuration
└── README.md # Project documentation

## Build and Run
### Prerequisites
- GCC compiler  
- Linux operating system (for `mprotect`, `seccomp`, and signal support)
- `make` utility

### Compilation
```bash
make
./miniOS
[BOOT] Initializing MiniOS...
[MEM] Heap initialized (16KB)
[FS] Mounted ROMFS image
[PROC] Forking sandboxed child...
[THREAD] Starting cooperative scheduler...
[SECURE] Sandboxing child with SECCOMP
