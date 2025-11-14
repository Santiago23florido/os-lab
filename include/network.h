#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>

/* Cooperative thread entry point that runs a simple TCP server on port 8888. */
void tcp_server_thread(void);

#endif /* NETWORK_H */
