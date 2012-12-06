#include <stdio.h>
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

    if (argc < 2) {
        printf("Usage: server <num>\n");
        return -1;
    }

    get_config("dht.cfg", &cfg);

    if ((cfg->num_servers % 2) == 1) {
        printf("There must be an ever number of servers!\n");
        goto err;
    }
    printf("Total servers: %u\n", cfg->num_servers);

    cfg->self_entry = atoi(argv[1]) - 1;
    if (cfg->self_entry > (cfg->num_servers - 1)) {
        printf("Invalid entry number!\n");
        goto err;
    }

    printf("Using entry %u from dht.cfg\n", cfg->self_entry);
    printf("Local address: %s:%s (%s)\n", cfg->servers[cfg->self_entry].address,
                                          cfg->servers[cfg->self_entry].port,
                                          cfg->servers[cfg->self_entry].is_primary ? "primary" : "secondary");

    if (0 != init_store()) {
        printf("Failed to initialize store\n");
        goto err;
    }
    
    listen_port = open_listening(cfg->servers[cfg->self_entry].port);

    accept_port = accept_connection(listen_port);

    while(1) {
        unsigned int cmd;
        unsigned int key_len, value_len;
        char *value;
        char key_hash[20];

        recv(accept_port, &cmd, 4, 0);

        switch (cmd) {
            case 0x00:   /* Store (key, value) pair */
                recv(accept_port, &key_len, 4, 0);
                recv_all(accept_port, packet_buf, key_len, 0);
                get_key(packet_buf, key_len, key_hash);

                recv(accept_port, &value_len, 4, 0);
                recv_all(accept_port, packet_buf, value_len, 0);

                put_pair(key_hash, packet_buf, value_len);
                break;

            case 0x01:   /* Read value (from key)   */
                recv(accept_port, &key_len, 4, 0);
                recv_all(accept_port, packet_buf, key_len, 0);
                get_key(packet_buf, key_len, key_hash);

                get_value(key_hash, &value, &value_len);
                *((unsigned int *) packet_buf) = value_len;
                memcpy(&packet_buf[4], value, value_len);

                send_all(accept_port, packet_buf, 4 + value_len);
                
                break;

            default:
                break;
        }
    }


    free_config(cfg);
    close_store();

    return 0;

err:
    free_config(cfg);
    return -1;
}
