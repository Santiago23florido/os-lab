
#ifndef COROUTINES_H
#define COROUTINES_H
#include <stdio.h>
#include <stddef.h>

typedef void* coroutine_t;
typedef void (*cofn)(void);

void enter_coroutine(coroutine_t cr);

void switch_coroutine(const coroutine_t *p_from, const coroutine_t to);

coroutine_t init_coroutine(const coroutine_t stack_begin, const size_t stack_size,const cofn initial_pc);

#endif

