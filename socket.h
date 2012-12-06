#ifndef __SOCKET_H
#define __SOCKET_H

#define BACKLOG 10
int open_listening(char *);
int accept_connection(int);
int open_connecting(char *, char *);

#endif
