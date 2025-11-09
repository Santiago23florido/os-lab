#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/mman.h>   // need for mprotect
#include <signal.h>
#include <ucontext.h>
#include "../include/terminal.h"
#include "../include/threads.h"


void init_terminal(int echo)
{
	struct termios settings;
	fcntl(0, F_SETFL, O_NONBLOCK);
	tcgetattr(0, &settings); /* grab old terminal i/o settings */
	settings.c_lflag &= ~ICANON; /* disable buffered i/o */
	settings.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
	tcsetattr(0, TCSANOW, &settings);
}
