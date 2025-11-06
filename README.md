
[THREAD] Starting cooperative scheduler...
[SECURE] Sandboxing child with SECCOMPMiniOS: A Modular Operating System Simulator in C

💡 Overview

MiniOS is a modular simulator written in C that integrates fundamental concepts of operating systems into a cohesive and tangible project. It is designed as a hands-on learning tool to study and demonstrate core OS topics such as memory management, file systems, threading, protection mechanisms, and inter-process communication.

Each module corresponds to a key concept explored in an undergraduate Operating Systems course, combining to form a small yet functional simulation of how a kernel operates internally.

✨ Features

ROMFS File System Module: Implements parsing and reading of a ROMFS-like filesystem image to simulate virtual disk mounting and file exploration.

Custom Memory Allocator: Provides a simple implementation of memalloc and memfree, using a linked-list allocator with coalescence to manage heap memory within the simulator.

Cooperative User-Level Scheduler: Simulates user-level cooperative threads using manual context switching, mimicking coroutine behavior and simple thread scheduling.

Memory Protection & Signal Handling: Utilizes the mprotect system call and POSIX signals to emulate memory isolation and error recovery between running threads.

Secure Process Sandbox: Demonstrates process isolation and inter-process communication (IPC) using pipe, fork, and seccomp to restrict a child process's access to underlying system calls.

📁 Project Structure (Visión General en Tabla)

El proyecto está dividido en los siguientes módulos principales, cada uno con su implementación (.c) y su interfaz (.h).

Componente

Archivos Principales

Descripción del Módulo

Core

src/main.c

Inicialización del sistema, integración de todos los módulos y secuencia de arranque principal.

File System

src/fs.c / fs.h

Implementa el parser y utilidades para simular el montaje y exploración de la imagen ROMFS.

Memory Manager

src/mem.c / mem.h

Contiene el asignador de memoria personalizado (mem_alloc/mem_free) con lógica de coalescencia.

Threading

src/threads.c / threads.h

Lógica de context switching para el planificador de hilos cooperativos.

Protection

src/protect.c / protect.h

Configura señales y utiliza mprotect para emular el aislamiento de memoria.

Sandboxing

src/sandbox.c / sandbox.h

Gestión de procesos (fork), IPC (pipe) y filtros de llamadas al sistema (seccomp).

Headers

include/

Directorio que contiene todos los archivos .h compartidos.

🛠️ Build and Run

Prerequisites

GCC/Clang: C compiler suite.

make utility.

Linux Operating System: Required for essential system calls like mprotect, seccomp, and POSIX signal support.

Compilation

make


Execution

./miniOS


Expected Output

[BOOT] Initializing MiniOS...
[MEM] Heap initialized (16KB)
[FS] Mounted ROMFS image
[PROC] Forking sandboxed child...
