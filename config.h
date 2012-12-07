#ifndef __CONFIG_H
#define __CONFIG_H

struct config {
    int num_servers;
    struct server_info *servers;
    int self_entry;
};

struct server_info {
    char *address;
    char *port;
    int socket_fd;
};

int get_config(char *, struct config **);
void free_config(struct config *);

#endif
