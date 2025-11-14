#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/mman.h>   // need for mprotect
#include <signal.h>
#include <ucontext.h>
#include <linux/seccomp.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <linux/seccomp.h>

#define BUFFER_SIZE 32

extern char input_buffer[BUFFER_SIZE];
extern char output_buffer[BUFFER_SIZE];
extern char* lastin;

extern int to_child_pipe[2];
extern int to_parent_pipe[2];

void pipes_init();
void producer();
void receiver();
void producer_init();
void receiver_init();
