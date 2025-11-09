#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/mman.h>   // need for mprotect
#include <signal.h>
#include <ucontext.h>

void init_terminal(int echo);


