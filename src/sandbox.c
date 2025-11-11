#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <ucontext.h>
#include <linux/seccomp.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <linux/seccomp.h>
#include "../include/sandbox.h"
#include "../include/threads.h"
#include "../include/coroutines.h"
#include "../include/fs.h"
#include <errno.h>

char input_buffer[BUFFER_SIZE];
char output_buffer[BUFFER_SIZE];
char* lastin = &input_buffer[0];

int to_child_pipe[2];
int to_parent_pipe[2];

static void flush_stdin_nonblock(void){
    int flags = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, flags | O_NONBLOCK);
    char d[256];
    while (read(0, d, sizeof d) > 0) {}
    fcntl(0, F_SETFL, flags);
}

void pipes_init(){
    pipe(to_child_pipe);
    pipe(to_parent_pipe);
    fcntl(to_child_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(to_child_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(to_parent_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(to_parent_pipe[1], F_SETFL, O_NONBLOCK);
}

void producer(){
    static char acc[BUFFER_SIZE];
    static size_t len = 0;
    char tmp[BUFFER_SIZE];
    for(;;){
        ssize_t n = read(0, tmp, sizeof(tmp));
        if (n < 0) { perror("read"); exit(EXIT_FAILURE); }
        if (n > 0) {
            if (len + (size_t)n > sizeof(acc)) { len = 0; }
            memcpy(acc + len, tmp, (size_t)n);
            len += (size_t)n;
            for (;;) {
                void *nlp = memchr(acc, '\n', len);
                if (!nlp) break;
                size_t linelen = (char*)nlp - acc + 1;
                printf("Read form producer: %.*s", (int)linelen, acc);
                write(to_child_pipe[1], acc, linelen);
                memmove(acc, acc + linelen, len - linelen);
                len -= linelen;
            }
        }
        yield1();
    }
}

void receiver(){
    static char acc[BUFFER_SIZE];
    static size_t len = 0;
    char tmp[BUFFER_SIZE];
    for(;;){
        ssize_t n = read(to_child_pipe[0], tmp, sizeof(tmp));
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) { yield1(); continue; }
            perror("read"); exit(EXIT_FAILURE);
        }
        if (n == 0) { yield1(); continue; }
        if (len + (size_t)n > sizeof(acc)) { len = 0; }
        memcpy(acc + len, tmp, (size_t)n);
        len += (size_t)n;

        for (;;) {
            void *nlp = memchr(acc, '\n', len);
            if (!nlp) break;
            size_t linelen = (char*)nlp - acc + 1;
            printf("Read, from son : %.*s", (int)linelen, acc);

            if (linelen == 3 && acc[0]=='l' && acc[1]=='s' && acc[2]=='\n') {
                flush_stdin_nonblock();
                printf("activating new thread\n");
                new_thread_needed = ls_wrapper;

            } else if (linelen >= 4 && (acc[0]=='+' || acc[0]=='-')) {
                const char* s = acc + 1;
                char* endp = NULL;
                long a = strtol(s, &endp, 10);
                if (endp == s || *endp != ',') goto _consume_only;
                s = endp + 1;
                long b = strtol(s, &endp, 10);
                if (endp != (char*)acc + linelen - 1 || *endp != '\n') goto _consume_only;

                param1 = (int)a;
                param2 = (int)b;
                if (acc[0] == '+') {
                    printf("activating addition (%d,%d)\n", param1, param2);
                    new_thread_needed = addition;
                } else {
                    printf("activating subtraction (%d,%d)\n", param1, param2);
                    new_thread_needed = substraction;
                }

            } else if (linelen >= 5 && memcmp(acc, "find ", 5) == 0) {
                const char* s = acc;
                const char* end = acc + linelen - 1;
                const char* namepos = strstr(s, " -name ");
                if (!namepos) goto _consume_only;
                s = namepos + 7;
                while (s < end && (*s == ' ' || *s == '\t')) s++;
                const char* startp = s;
                const char* endp2 = NULL;
                if (s < end && *s == '\"') {
                    startp = ++s;
                    endp2 = memchr(s, '\"', end - s);
                    if (!endp2) goto _consume_only;
                } else {
                    endp2 = s;
                    while (endp2 < end && *endp2 != ' ' && *endp2 != '\t') endp2++;
                }
                size_t flen = (size_t)(endp2 - startp);
                if (flen == 0) goto _consume_only;
                if (fname) { free(fname); fname = NULL; }
                fname = (char*)malloc(flen + 1);
                if (!fname) { perror("malloc"); exit(EXIT_FAILURE); }
                memcpy(fname, startp, flen);
                fname[flen] = '\0';
                printf("activating find for \"%s\"\n", fname);
                new_thread_needed = find_wrapper;
            }

        _consume_only:
            memmove(acc, acc + linelen, len - linelen);
            len -= linelen;
        }
        yield1();
    }
}



void producer_init(){
    coroutine_t prod = init_coroutine(stacks[0],STACK_SIZE,producer);
    thread_init(prod,0,5);
}
void receiver_init(){
    coroutine_t rec = init_coroutine(stacks[1],STACK_SIZE,receiver);
    thread_init(rec,1,3);
}
