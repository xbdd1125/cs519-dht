#include <string.h>

#include "socket.h"

int dht_connect(char *addr, char *port) {
    return open_connecting(addr, port);
}

void dht_write(int fd, char *key, char *val) {
    send_cmd(fd, PUT);
    send_string(fd, key, strlen(key) + 1);
    send_string(fd, val, strlen(val) + 1);
    recv_ok(fd);
}

int dht_read(int fd, char *key, char *val) {
    int value_len;

    send_cmd(fd , GET);
    send_string(fd, key, strlen(key) + 1);
    recv_string(fd, val, &value_len);

    return value_len;
}

void dht_disconnect(int fd) {
    close_connection(fd);
}

