#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "key.h"
#include "store.h"
#include "socket.h"

static struct config *cfg;

static char key_buf[8192];
static char val_buf[8192];


static void handle_primary(int cmd, int sockfd) {
    unsigned int key_len, value_len;
    int my_index = cfg->self_entry,
        key_index;
    char *value;
    char key_hash[20];

    int otherfd;

    switch (cmd) {
        case PUT:   /* Store (key, value) pair */

            /* First string is key */
            recv_string(sockfd, key_buf, &key_len);
            get_key(key_buf, key_len, key_hash);

            /* Second string is value */
            recv_string(sockfd, val_buf, &value_len);

            key_index = who_has_key(key_hash, cfg->num_servers);

            if (my_index == key_index * 2) {
                /* Put them in the store */
                put_pair(key_hash, val_buf, value_len);

                /* Replicate to secondary... */
                otherfd = open_connecting(cfg->servers[my_index + 1].address,
                                          cfg->servers[my_index + 1].port);

                send_cmd(otherfd, PUT);
                send_string(otherfd, key_buf, key_len);
                send_string(otherfd, val_buf, value_len);
                recv_ok(otherfd);

                close_connection(otherfd);
            }

            else {
                /* Send them to someone else */
                otherfd = open_connecting(cfg->servers[key_index * 2].address,
                                          cfg->servers[key_index * 2].port);
                send_cmd(otherfd, PUT);
                send_string(otherfd, key_buf, key_len);
                send_string(otherfd, val_buf, value_len);
                recv_ok(otherfd);

                close_connection(otherfd);
            }

            send_cmd(sockfd, OK);

            /* Send confirmation to client */
            break;


        case GET:   /* Read value (from key)   */

            /* First string is key */
            recv_string(sockfd, key_buf, &key_len);
            get_key(key_buf, key_len, key_hash);

            key_index = who_has_key(key_hash, cfg->num_servers);

            if (my_index == key_index * 2) {
                /* Get matching value and send it back */
                get_value(key_hash, &value, &value_len);
            }
            else {
                /* Get it from someone else */
                otherfd = open_connecting(cfg->servers[key_index * 2].address,
                                          cfg->servers[key_index * 2].port);
                send_cmd(otherfd , GET);
                send_string(otherfd, key_buf, key_len);
                recv_string(otherfd, val_buf, &value_len);
                value = val_buf;

                close_connection(otherfd);
            }

            send_string(sockfd, value, value_len);
            break;
    
        /* We (primaries) don't respond to other messages */
        default:
            break;
    }
}


static void handle_secondary(int cmd, int sockfd) {
    unsigned int key_len, value_len;
    char *value;
    char key_hash[20];

    switch (cmd) {
        case PUT:   /* Store (key, value) pair */

            /* First string is key */
            recv_string(sockfd, key_buf, &key_len);
            get_key(key_buf, key_len, key_hash);

            /* Second string is value */
            recv_string(sockfd, val_buf, &value_len);
            put_pair(key_hash, val_buf, value_len);

            /* Send confirmation to client */
            send_cmd(sockfd, OK);
            break;


        case GET:   /* Read value (from key)   */

            /* First string is key */
            recv_string(sockfd, key_buf, &key_len);
            get_key(key_buf, key_len, key_hash);

            get_value(key_hash, &value, &value_len);
            send_string(sockfd, value, value_len);
            break;
    
        /* We (secondaries) should respond to active/standby messages... */
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

    if ((cfg->self_entry % 2) == 0)
        handle_connections(listen_port, &handle_primary);
    else
        handle_connections(listen_port, &handle_secondary);
   
    free_config(cfg);
    close_store();

    return 0;

err:
    free_config(cfg);
    return -1;
}
