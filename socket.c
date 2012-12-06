#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "socket.h"

int open_listening(char *port) {
    int status, sockfd;
    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }
    
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    listen(sockfd, BACKLOG);

    freeaddrinfo(res);

    return sockfd;
}


int accept_connection(int listen_socket) {
    struct sockaddr_storage incoming_addr;
    socklen_t addr_size;

    addr_size = sizeof incoming_addr;
    
    return accept(listen_socket, (struct sockaddr *)&incoming_addr, &addr_size);
}


int open_connecting(char *address, char *port) {
    int status, sockfd;
    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(address, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }
    
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(sockfd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    return sockfd;
}


void close_connection(int sockfd) {
    close(sockfd);
}


static int send_all(int sockfd, char *buffer, size_t len)
{
    int sent = 0;
    int left = len;
    int status = 0;

    while(sent < len) {
        status = send(sockfd, buffer + sent, left, 0);
        if (status == -1) break;
        sent = sent + status;
        left = left - status;
    }
    return status;
} 


static int recv_all(int sockfd, char *buffer, size_t len) {
    int recvd = 0;
    int left  = len;
    int status = 0;

    while (recvd < len) {
        status = recv(sockfd, buffer + recvd, left, 0);
        if (status == -1) break;
        recvd = recvd + status;
        left  = left  - status;
    }
    return status;
}


void recv_string(int sockfd, char *buf, unsigned int *len) {
    recv(sockfd, len, 4, 0);
    recv_all(sockfd, buf, *len);
}

void send_string(int sockfd, char *buf, unsigned int len) {
    send(sockfd, &len, 4, 0);
    send_all(sockfd, buf, len);
}

void send_cmd(int sockfd, int cmd) {
    send(sockfd, &cmd, 4, 0);
}


void handle_connections(int listen_socket, void (*handle_cmd)(int, int)) {

    fd_set readfds, master;
    int maxfd;

    FD_ZERO(&master);
    FD_SET(listen_socket, &master);
    maxfd = listen_socket;

    while(1) {
        int i, newfd, status;
        unsigned int cmd;


        readfds = master;

        if (-1 == select(maxfd + 1, &readfds, NULL, NULL, NULL)) {
            perror("select failed");
            exit(1);
        }

        for (i = 0; i <= maxfd; i++) {
            if (FD_ISSET(i, &readfds)) {

                /* New connection */
                if (i == listen_socket) {
                    newfd = accept_connection(listen_socket);

                    if (-1 == newfd) 
                        perror("new connection accept failed");
                    else {
                        FD_SET(newfd, &master);
                        maxfd = (newfd > maxfd) ? newfd : maxfd;
                    }
                }
                
                /* New data from existing connection */
                else {

                    status = recv(i, &cmd, 4, 0);

                    if (status <= 0) {
                        close(i);
                        FD_CLR(i, &master);
                        continue;
                    }

                    (*handle_cmd)(cmd, i);
                }
            }
        }
    }
}
