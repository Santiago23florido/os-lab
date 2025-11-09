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
#include "../include/sandbox.h"
#include "../include/threads.h"


char input_buffer[BUFFER_SIZE];
char output_buffer[BUFFER_SIZE];
char* lastin = &input_buffer[0];

int to_child_pipe[2];
int to_parent_pipe[2];


void pipes_init(){
    pipe(to_child_pipe);
    pipe(to_parent_pipe);
}

void producer(){
	for(;;){
		yield();
        prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);
		int c = getchar();
        if (c != -1) {
            char ch = (char)c;               
            write(to_child_pipe[1], &ch, 1); 
        } 
    }		
	}