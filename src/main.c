#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/mman.h>   // need for mprotect
#include <signal.h>
#include <ucontext.h>
#include "../include/mem.h"
#include "../include/fs.h"
#include "../include/terminal.h"
#include "../include/sandbox.h"
#include "../include/threads.h"

int main(){
    meminit();
    const char *path = "tp1fs.romfs";
    fs_mount(path);
    init_terminal(0);
    pipes_init();
    init_scheduler();
    return 0;
}
