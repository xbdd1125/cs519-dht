#include <stdio.h>
#include <string.h>

#include "socket.h"

int main(int argc, char **argv) {

    char packet_buf[8192];
    unsigned int len;
    char *data;
    int connect_port;

    if (argc < 3) {
        printf("Usage: client <addr> <port>\n");
        return -1;
    }

    connect_port = open_connecting(argv[1], argv[2]);

    *((unsigned int *) packet_buf)     = 0x00;  /* Store */
    *((unsigned int *) &packet_buf[4]) = 4;
    memcpy(&packet_buf[8], "1234", 4);
    *((unsigned int *) &packet_buf[12]) = 12;
    memcpy(&packet_buf[16], "abcdefghijk\0", 12);

    send_all(connect_port, packet_buf, 4 + 4 + 4 + 4 + 12);

    *((unsigned int *) packet_buf)     = 0x01;  /* Get */
    *((unsigned int *) &packet_buf[4]) = 4;
    memcpy(&packet_buf[8], "1234", 4);

    send_all(connect_port, packet_buf, 4 + 4 + 4);
    
    recv(connect_port, &len, 4, 0);

    recv_all(connect_port, packet_buf, len);

    printf("Got: %s\n", packet_buf);

    return 0;
}
