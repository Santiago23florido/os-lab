#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include "../include/threads.h"


extern int g_sesgv;

void protect_stacks();
void sigsegv_handler(int sig);