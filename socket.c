#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

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
        printf("getaddrinfo error: %s\n", gai_strerror(status));
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
        printf("getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }
    
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(sockfd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    return sockfd;
}


int send_all(int sockfd, char *buffer, size_t len)
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


int recv_all(int sockfd, char *buffer, size_t len) {
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
