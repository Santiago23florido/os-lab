#include <sys/mman.h>
#include <unistd.h>
#include "../include/threads.h"
#include "../include/protect.h"


int g_sesgv=0; 

void protect_stacks(){
    for(int i = 0;i<THREADLIMIT;i++){
        mprotect(stacks[i], STACK_SIZE, PROT_NONE);      
    }
    mprotect(stacks[THREADLIMIT], STACK_SIZE, PROT_READ | PROT_WRITE);
};
void sigsegv_handler(int sig){
    const char msg[] = "[!] SIGSEGV detected\n";
    (void)sig;
    write(STDERR_FILENO, msg, sizeof msg - 1);
    g_sesgv=1;
    enter_coroutine(schedulerglobal->coroutine);
}