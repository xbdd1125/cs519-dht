#ifndef __CLIENT_API_H
#define __CLIENT_API_H

int dht_connect(char *, char *);
int dht_write(int, char *, char *);
int dht_read(int, char *, char *);
void dht_disconnect(void);

#endif
