#ifndef __SOCKET_H
#define __SOCKET_H

#define BACKLOG 10

int open_listening(char *);
int accept_connection(int);
int open_connecting(char *, char *);
void close_connection(int);
void handle_connections(int, int (*handle_cmd)(int, int));
void send_string(int, char *, unsigned int);
void recv_string(int, char *, unsigned int *);
void send_cmd(int, int);
void recv_ok(int);

#define GET   0x00
#define PUT   0x01
#define SLEEP 0x02
#define WAKE  0x03
#define KILL  0x04
#define DIE   0x05

#define OK    0xff

#endif
