#include <stdio.h>
#include <string.h>

#include "socket.h"

int main(int argc, char **argv) {
    int i;
    char buf[8192];
    char value_string[] = "abcdefg_";

    unsigned int len;
    char *data;
    int connect_port;

    if (argc < 3) {
        printf("Usage: client <addr> <port>\n");
        return -1;
    }

    connect_port = open_connecting(argv[1], argv[2]);
    
    for (i = 0; i < 10; i++) {
        
        value_string[7] = '0' + i;

        send_cmd(connect_port, PUT);
        send_string(connect_port, (char *) &i, sizeof(int));
        send_string(connect_port, value_string, strlen(value_string));
    }
    
    i = 5;
    send_cmd(connect_port, GET);
    send_string(connect_port, (char *) &i, sizeof(int));
    recv_string(connect_port, buf, &len);

    printf("Got: %s\n", buf);

    return 0;
}
