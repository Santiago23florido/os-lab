#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../include/network.h"
#include "../include/threads.h"

void tcp_server_thread(void) {
    int sockfd = -1;
    int newsockfd = -1;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        goto finish;
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        goto finish;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8888);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        goto finish;
    }

    if (listen(sockfd, 5) < 0) {
        perror("listen");
        goto finish;
    }

    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    }

    printf("TCP server listening on port 8888...\n");
    fflush(stdout);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd >= 0) {
            break;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            yield1();
            continue;
        }
        perror("accept");
        goto finish;
    }

    flags = fcntl(newsockfd, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(newsockfd, F_SETFL, flags | O_NONBLOCK);
    }

    size_t total = 0;
    while (total < sizeof(buffer) - 1) {
        ssize_t n = read(newsockfd, buffer + total, sizeof(buffer) - 1 - total);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                yield1();
                continue;
            }
            perror("read");
            goto finish;
        }
        if (n == 0) break;
        total += (size_t)n;
        if (memchr(buffer, '\n', total) != NULL) break;
    }

    buffer[total] = '\0';
    printf("Here is the message: %s\n", buffer);
    fflush(stdout);

    const char reply[] = "I got your message";
    size_t sent = 0;
    while (sent < sizeof(reply) - 1) {
        ssize_t n = write(newsockfd, reply + sent, (sizeof(reply) - 1) - sent);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                yield1();
                continue;
            }
            perror("write");
            break;
        }
        sent += (size_t)n;
    }

finish:
    if (newsockfd >= 0) close(newsockfd);
    if (sockfd >= 0) close(sockfd);
    schedulerglobal->current->priority = 0;
    schedulerglobal->current->state = FINISHED;
    while (1) yield1();
}
