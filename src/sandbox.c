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
#include "../include/coroutines.h"


char input_buffer[BUFFER_SIZE];
char output_buffer[BUFFER_SIZE];
char* lastin = &input_buffer[0];

int to_child_pipe[2];
int to_parent_pipe[2];


void pipes_init(){
    pipe(to_child_pipe);
    pipe(to_parent_pipe);
    fcntl(to_child_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(to_child_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(to_parent_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(to_parent_pipe[1], F_SETFL, O_NONBLOCK);
}

void producer(){
	for(;;){
		ssize_t nbytes = read(0,input_buffer,BUFFER_SIZE - 1);
        if (nbytes < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }   
        input_buffer[nbytes] = '\0';
        printf("Leído, desde el padre: %s\n", input_buffer);
        write(to_child_pipe[1],input_buffer,nbytes);     
        yield1();
    }		
}

void receiver(){
    for(;;){
        ssize_t nbytes = read(to_child_pipe[0],output_buffer,BUFFER_SIZE - 1);
        if (nbytes < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        output_buffer[nbytes] = '\0';
        printf("Leído, desde el hijo: %s\n", output_buffer);
        yield1();
    }
}

void producer_init(){
    coroutine_t prod = init_coroutine(stacks[0],STACK_SIZE,producer);
    thread_init(prod,5);
}
void receiver_init(){
    coroutine_t rec = init_coroutine(stacks[1],STACK_SIZE,receiver);
    thread_init(rec,3);
}