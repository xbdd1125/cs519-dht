#include <stdio.h>
#include <string.h>

#include "socket.h"

int main(int argc, char **argv) {

    char buf[8192];
    unsigned int len;
    char *data;
    int connect_port;

    if (argc < 3) {
        printf("Usage: client <addr> <port>\n");
        return -1;
    }

    connect_port = open_connecting(argv[1], argv[2]);
    
    send_cmd(connect_port, PUT);
    send_string(connect_port, "1234", 4);
    send_string(connect_port, "abcdefghijk\0", 12);

    send_cmd(connect_port, GET);
    send_string(connect_port, "1234", 4);
    recv_string(connect_port, buf, &len);

    printf("Got: %s\n", buf);

    return 0;
}
