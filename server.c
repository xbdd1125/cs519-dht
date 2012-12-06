#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "key.h"
#include "store.h"
#include "socket.h"

static struct config *cfg;
static char buf[8192];

static void handle_primary(int cmd, int sockfd) {
    unsigned int key_len, value_len;
    char *value;
    char key_hash[20];

    switch (cmd) {
        case PUT:   /* Store (key, value) pair */

            /* First string is key */
            recv_string(sockfd, buf, &key_len);
            get_key(buf, key_len, key_hash);

            /* Second string is value */
            recv_string(sockfd, buf, &value_len);

            /* Put them in the store */
            put_pair(key_hash, buf, value_len);

            break;

        case GET:   /* Read value (from key)   */

            /* First string is key */
            recv_string(sockfd, buf, &key_len);
            get_key(buf, key_len, key_hash);

            /* Get matching value and send it back */
            get_value(key_hash, &value, &value_len);
            send_string(sockfd, value, value_len);
            break;
    
        /* We don't respond to other messages */
        default:
            break;
    }
}

int main(int argc, char **argv) {

    int listen_port;
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
    printf("Local address: %s:%s (%s)\n\n", 
            cfg->servers[cfg->self_entry].address,
            cfg->servers[cfg->self_entry].port,
            cfg->servers[cfg->self_entry].is_primary ? "primary" : "secondary");

    if (0 != init_store()) {
        fprintf(stderr, "Failed to initialize store\n");
        goto err;
    }
    
    listen_port = open_listening(cfg->servers[cfg->self_entry].port);

    /* This loops */
    handle_connections(listen_port, &handle_primary);
   
    free_config(cfg);
    close_store();

    return 0;

err:
    free_config(cfg);
    return -1;
}
