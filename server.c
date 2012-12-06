#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "key.h"
#include "store.h"
#include "socket.h"

int main(int argc, char **argv) {

    struct config *cfg;
    char hash[20];
    char packet_buf[8192];
    char *data;
    size_t len;
    int listen_port,
        accept_port;
    fd_set readfds, master;
    int maxfd;

    if (argc < 2) {
        printf("Usage: server <num>\n");
        return -1;
    }

    get_config("dht.cfg", &cfg);

    if ((cfg->num_servers % 2) == 1) {
        fprintf(stderr, "There must be an ever number of servers!\n");
        goto err;
    }
    printf("Total servers: %u\n", cfg->num_servers);

    cfg->self_entry = atoi(argv[1]) - 1;
    if (cfg->self_entry > (cfg->num_servers - 1)) {
        fprintf(stderr, "Invalid entry number!\n");
        goto err;
    }

    printf("Using entry %u from dht.cfg\n", cfg->self_entry);
    printf("Local address: %s:%s (%s)\n", cfg->servers[cfg->self_entry].address,
                                          cfg->servers[cfg->self_entry].port,
                                          cfg->servers[cfg->self_entry].is_primary ? "primary" : "secondary");

    if (0 != init_store()) {
        fprintf(stderr, "Failed to initialize store\n");
        goto err;
    }
    
    listen_port = open_listening(cfg->servers[cfg->self_entry].port);
    
    FD_ZERO(&master);
    FD_SET(listen_port, &master);
    maxfd = listen_port;

    while(1) {
        int i, newfd, status;
        unsigned int cmd;
        unsigned int key_len, value_len;
        char *value;
        char key_hash[20];

        readfds = master;

        if (-1 == select(maxfd + 1, &readfds, NULL, NULL, NULL)) {
            perror("select failed");
            exit(1);
        }

        for (i = 0; i <= maxfd; i++) {
            if (FD_ISSET(i, &readfds)) {

                /* New connection */
                if (i == listen_port) {
                    newfd = accept_connection(listen_port);

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

                    switch (cmd) {
                        case 0x00:   /* Store (key, value) pair */
                            recv(i, &key_len, 4, 0);
                            recv_all(i, packet_buf, key_len, 0);
                            get_key(packet_buf, key_len, key_hash);

                            recv(i, &value_len, 4, 0);
                            recv_all(i, packet_buf, value_len, 0);

                            put_pair(key_hash, packet_buf, value_len);
                            break;

                        case 0x01:   /* Read value (from key)   */
                            recv(i, &key_len, 4, 0);
                            recv_all(i, packet_buf, key_len, 0);
                            get_key(packet_buf, key_len, key_hash);

                            get_value(key_hash, &value, &value_len);
                            *((unsigned int *) packet_buf) = value_len;
                            memcpy(&packet_buf[4], value, value_len);

                            send_all(i, packet_buf, 4 + value_len);
                            
                            break;

                        default:
                            break;
                    }
                }
            }
        }
    }

    free_config(cfg);
    close_store();

    return 0;

err:
    free_config(cfg);
    return -1;
}
